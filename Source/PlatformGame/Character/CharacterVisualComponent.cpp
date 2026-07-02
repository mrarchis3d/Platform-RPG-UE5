// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterVisualComponent.h"
#include "../PlatformGameLogs.h"
#include "ECharacterSlots.h"
#include "Data/BodyDataAsset.h"
#include "Data/EquipmentDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"


void UCharacterVisualComponent::SetTargetSkeleton(USkeletalMeshComponent* InSkeleton)
{
    TargetSkeleton = InSkeleton;
}

void UCharacterVisualComponent::InitializeBody(UBodyDataAsset* BodyData)
{
    if (!BodyData)
    {
        UE_LOG(LogCharacterVisual, Warning, TEXT("[CharacterVisual] InitializeBody called with null BodyDataAsset!"));
        return;
    }

    DestroyAllPlanes();
    HiddenByEquipment.Empty();

    ActiveBodyData = BodyData;
    BodyMaterialTemplate = BodyData->BaseMaterialTemplate;

    EnsurePlaneMeshLoaded();

    for (const auto& Pair : BodyData->BodySlots)
    {
        const EBodySlotType SlotType = Pair.Key;
        const FBodySlotData& SlotData = Pair.Value;

        UStaticMeshComponent* Plane = CreateSlotPlane(
            SlotData.SocketName,
            SlotData,
            SlotData.ZOrder
        );

        if (Plane)
        {
            UMaterialInstanceDynamic* DMI = CreateBodySlotMaterial(
                SlotData.MaterialTemplate ? SlotData.MaterialTemplate : BodyMaterialTemplate,
                Plane
            );

            BodyPlanes.Add(SlotType, Plane);
            if (DMI)
            {
                BodyMaterials.Add(SlotType, DMI);
            }
        }
    }
}

void UCharacterVisualComponent::EquipVisual(EEquipmentSlotType Slot, UEquipmentDataAsset* EquipmentData)
{
    if (!EquipmentData)
    {
        UE_LOG(LogCharacterVisual, Warning, TEXT("[CharacterVisual] EquipVisual called with null EquipmentDataAsset for slot %s!"),
            *UEnum::GetValueAsString(Slot));
        return;
    }

    if (EquipmentPlanes.Contains(Slot))
    {
        UnequipVisual(Slot);
    }

    EnsurePlaneMeshLoaded();

    const FEquipmentSlotData& SlotConfig = EquipmentData->SlotConfig;

    UStaticMeshComponent* Plane = CreateSlotPlane(
        SlotConfig.SocketName,
        SlotConfig,
        SlotConfig.ZOrder
    );

    if (!Plane)
    {
        UE_LOG(LogCharacterVisual, Warning, TEXT("[CharacterVisual] Failed to create plane for equipment %s in slot %s!"),
            *EquipmentData->EquipmentID.ToString(), *UEnum::GetValueAsString(Slot));
        return;
    }

    UMaterialInstanceDynamic* DMI = CreateEquipmentSlotMaterial(
        EquipmentData->MaterialTemplate,
        Plane
    );

    EquipmentPlanes.Add(Slot, Plane);
    if (DMI)
    {
        EquipmentMaterials.Add(Slot, DMI);
    }

    for (const EBodySlotType HiddenSlot : SlotConfig.HiddenBodySlots)
    {
        HideBodySlot(HiddenSlot, Slot);
    }
}

void UCharacterVisualComponent::UnequipVisual(EEquipmentSlotType Slot)
{
    TArray<EBodySlotType> SlotsToReveal;
    for (const auto& Pair : HiddenByEquipment)
    {
        if (Pair.Value == Slot)
        {
            SlotsToReveal.Add(Pair.Key);
        }
    }

    for (const EBodySlotType HiddenSlot : SlotsToReveal)
    {
        RevealBodySlot(HiddenSlot);
    }

    if (TObjectPtr<UStaticMeshComponent>* PlanePtr = EquipmentPlanes.Find(Slot))
    {
        if (UStaticMeshComponent* Plane = PlanePtr->Get())
        {
            Plane->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
            Plane->DestroyComponent();
        }
        EquipmentPlanes.Remove(Slot);
    }

    EquipmentMaterials.Remove(Slot);
}

void UCharacterVisualComponent::ClearAllEquipmentVisuals()
{
    TArray<EEquipmentSlotType> Slots;
    EquipmentPlanes.GetKeys(Slots);

    for (const EEquipmentSlotType Slot : Slots)
    {
        UnequipVisual(Slot);
    }
}

void UCharacterVisualComponent::SetBodyPlaneVisible(EBodySlotType Slot, bool bShow)
{
    if (TObjectPtr<UStaticMeshComponent>* PlanePtr = BodyPlanes.Find(Slot))
    {
        if (UStaticMeshComponent* Plane = PlanePtr->Get())
        {
            Plane->SetVisibility(bShow, true);
        }
    }
}

void UCharacterVisualComponent::RefreshBodySlotVisibility(EBodySlotType Slot)
{
    const bool bIsHidden = HiddenByEquipment.Contains(Slot);
    SetBodyPlaneVisible(Slot, !bIsHidden);
}


void UCharacterVisualComponent::DestroyAllPlanes()
{
    for (auto& Pair : BodyPlanes)
    {
        if (UStaticMeshComponent* Plane = Pair.Value)
        {
            Plane->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
            Plane->DestroyComponent();
        }
    }
    BodyPlanes.Empty();
    BodyMaterials.Empty();

    for (auto& Pair : EquipmentPlanes)
    {
        if (UStaticMeshComponent* Plane = Pair.Value)
        {
            Plane->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
            Plane->DestroyComponent();
        }
    }
    EquipmentPlanes.Empty();
    EquipmentMaterials.Empty();

    HiddenByEquipment.Empty();
}

UStaticMeshComponent* UCharacterVisualComponent::CreateSlotPlane(
    const FName& SocketName,
    const FCharacterSlotConfig& Config,
    int32 ZOrder
)
{
    if (!PlaneMesh)
    {
        return nullptr;
    }

    AActor* Owner = GetOwner();
    if (!Owner)
    {
        return nullptr;
    }

    UStaticMeshComponent* Plane = NewObject<UStaticMeshComponent>(Owner);
    if (!Plane)
    {
        return nullptr;
    }

    Plane->SetStaticMesh(PlaneMesh);
    Plane->SetMaterial(0, FallbackMaterial);
    Plane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Plane->CastShadow = false;
    Plane->bHiddenInGame = false;

    // Convert pixel dimensions to plane scale:
    // WorldSize = Pixels / PPU, then Scale = WorldSize / 100 (plane is 100 UU)
    const float EffectivePPU = FMath::Max(PixelsPerUnit, 0.01f);
    const FVector PlaneScale(
        Config.PlaneSize.X / (EffectivePPU * 100.f),
        Config.PlaneSize.Y / (EffectivePPU * 100.f),
        1.f
    );
    Plane->SetRelativeScale3D(PlaneScale);

    Plane->TranslucencySortPriority = ZOrder;

    // Attach to the skeleton bones if available, otherwise fall back to root
    USceneComponent* AttachTarget = TargetSkeleton
        ? static_cast<USceneComponent*>(TargetSkeleton)
        : Owner->GetRootComponent();

    // --- Diagnostic logging ---
    if (!TargetSkeleton)
    {
        UE_LOG(LogCharacterVisual, Warning,
            TEXT("[CharacterVisual] CreateSlotPlane: TargetSkeleton is NULL! "
                 "Did you call SetTargetSkeleton before InitializeBody? "
                 "Falling back to root component."));
    }
    else if (!SocketName.IsNone() && !TargetSkeleton->DoesSocketExist(SocketName))
    {
        UE_LOG(LogCharacterVisual, Warning,
            TEXT("[CharacterVisual] CreateSlotPlane: Socket '%s' does NOT exist "
                 "on skeleton '%s'! Plane will attach to skeleton origin."),
            *SocketName.ToString(),
            *TargetSkeleton->GetName());
    }

    Plane->RegisterComponent();

    Plane->AttachToComponent(
        AttachTarget,
        FAttachmentTransformRules::SnapToTargetNotIncludingScale,
        SocketName
    );

    Plane->SetRelativeLocation(Config.RelativeLocation);
    Plane->SetRelativeRotation(Config.RelativeRotation);

    UE_LOG(LogCharacterVisual, Log,
        TEXT("[CharacterVisual] Plane for socket '%s' → WorldPos: %s | RelPos: %s | Scale: %s"),
        *SocketName.ToString(),
        *Plane->GetComponentLocation().ToString(),
        *Plane->GetRelativeLocation().ToString(),
        *Plane->GetRelativeScale3D().ToString());

    return Plane;
}

UMaterialInstanceDynamic* UCharacterVisualComponent::CreateBodySlotMaterial(
    UMaterialInterface* Template,
    UStaticMeshComponent* Plane
)
{
    UMaterialInterface* EffectiveTemplate = Template ? Template : BodyMaterialTemplate.Get();
    if (!EffectiveTemplate)
    {
        EffectiveTemplate = FallbackMaterial.Get();
    }

    if (!EffectiveTemplate || !Plane)
    {
        return nullptr;
    }

    UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(EffectiveTemplate, this);
    if (DMI)
    {
        Plane->SetMaterial(0, DMI);
    }

    return DMI;
}

UMaterialInstanceDynamic* UCharacterVisualComponent::CreateEquipmentSlotMaterial(
    UMaterialInterface* Template,
    UStaticMeshComponent* Plane
)
{
    if (!Template)
    {
        Template = FallbackMaterial.Get();
    }

    if (!Template || !Plane)
    {
        return nullptr;
    }

    UMaterialInstanceDynamic* DMI = UMaterialInstanceDynamic::Create(Template, this);
    if (DMI)
    {
        Plane->SetMaterial(0, DMI);
    }

    return DMI;
}

void UCharacterVisualComponent::HideBodySlot(EBodySlotType Slot, EEquipmentSlotType ByEquipment)
{
    HiddenByEquipment.Add(Slot, ByEquipment);
    SetBodyPlaneVisible(Slot, false);
}

void UCharacterVisualComponent::RevealBodySlot(EBodySlotType Slot)
{
    HiddenByEquipment.Remove(Slot);
    SetBodyPlaneVisible(Slot, true);
}

void UCharacterVisualComponent::EnsurePlaneMeshLoaded()
{
    if (PlaneMesh)
    {
        return;
    }

    PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
    if (!PlaneMesh)
    {
        UE_LOG(LogCharacterVisual, Error, TEXT("[CharacterVisual] Failed to load /Engine/BasicShapes/Plane!"));
    }
}

// ── Debug Tools ───────────────────────────────────────────────────────────────

void UCharacterVisualComponent::DebugAdjustBodySlot(
    EBodySlotType Slot,
    FVector NewLocation,
    FRotator NewRotation
)
{
    if (TObjectPtr<UStaticMeshComponent>* PlanePtr = BodyPlanes.Find(Slot))
    {
        if (UStaticMeshComponent* Plane = PlanePtr->Get())
        {
            Plane->SetRelativeLocation(NewLocation);
            Plane->SetRelativeRotation(NewRotation);

            UE_LOG(LogCharacterVisual, Log,
                TEXT("[Debug] Body slot %s → Relative: %s, %s | World: %s"),
                *UEnum::GetValueAsString(Slot),
                *NewLocation.ToString(),
                *NewRotation.ToString(),
                *Plane->GetComponentLocation().ToString());
        }
    }
    else
    {                                                                        
        UE_LOG(LogCharacterVisual, Warning,
            TEXT("[Debug] Body slot %s not found in BodyPlanes!"),
            *UEnum::GetValueAsString(Slot));
    }
}

void UCharacterVisualComponent::DebugAdjustEquipmentSlot(
    EEquipmentSlotType Slot,
    FVector NewLocation,
    FRotator NewRotation
)
{
    if (TObjectPtr<UStaticMeshComponent>* PlanePtr = EquipmentPlanes.Find(Slot))
    {
        if (UStaticMeshComponent* Plane = PlanePtr->Get())
        {
            Plane->SetRelativeLocation(NewLocation);
            Plane->SetRelativeRotation(NewRotation);

            UE_LOG(LogCharacterVisual, Log,
                TEXT("[Debug] Equipment slot %s → Relative: %s, %s | World: %s"),
                *UEnum::GetValueAsString(Slot),
                *NewLocation.ToString(),
                *NewRotation.ToString(),
                *Plane->GetComponentLocation().ToString());
        }
    }
    else
    {
        UE_LOG(LogCharacterVisual, Warning,
            TEXT("[Debug] Equipment slot %s not found in EquipmentPlanes!"),
            *UEnum::GetValueAsString(Slot));
    }
}

void UCharacterVisualComponent::DebugPrintAllSlotTransforms()
{
    UE_LOG(LogCharacterVisual, Log, TEXT("===== [Debug] Body Slot Transforms ====="));
    for (const auto& Pair : BodyPlanes)
    {
        if (const UStaticMeshComponent* Plane = Pair.Value)
        {
            const FName ParentSocket = Plane->GetAttachSocketName();
            UE_LOG(LogCharacterVisual, Log,
                TEXT("  %s → Socket: %s | RelLoc: %s | RelRot: %s | WorldLoc: %s | Scale: %s"),
                *UEnum::GetValueAsString(Pair.Key),
                *ParentSocket.ToString(),
                *Plane->GetRelativeLocation().ToString(),
                *Plane->GetRelativeRotation().ToString(),
                *Plane->GetComponentLocation().ToString(),
                *Plane->GetRelativeScale3D().ToString());
        }
    }

    UE_LOG(LogCharacterVisual, Log, TEXT("===== [Debug] Equipment Slot Transforms ====="));
    for (const auto& Pair : EquipmentPlanes)
    {
        if (const UStaticMeshComponent* Plane = Pair.Value)
        {
            const FName ParentSocket = Plane->GetAttachSocketName();
            UE_LOG(LogCharacterVisual, Log,
                TEXT("  %s → Socket: %s | RelLoc: %s | RelRot: %s | WorldLoc: %s | Scale: %s"),
                *UEnum::GetValueAsString(Pair.Key),
                *ParentSocket.ToString(),
                *Plane->GetRelativeLocation().ToString(),
                *Plane->GetRelativeRotation().ToString(),
                *Plane->GetComponentLocation().ToString(),
                *Plane->GetRelativeScale3D().ToString());
        }
    }

    UE_LOG(LogCharacterVisual, Log,
        TEXT("  TargetSkeleton: %s | PPU: %.2f"),
        TargetSkeleton ? *TargetSkeleton->GetName() : TEXT("NULL"),
        PixelsPerUnit);
}

void UCharacterVisualComponent::DebugSetPixelsPerUnit(float NewPPU)
{
    PixelsPerUnit = FMath::Max(NewPPU, 0.01f);
    const float EffectivePPU = PixelsPerUnit;

    // Re-scale all existing body planes
    if (ActiveBodyData)
    {
        for (auto& Pair : BodyPlanes)
        {
            if (const FBodySlotData* SlotData = ActiveBodyData->BodySlots.Find(Pair.Key))
            {
                if (UStaticMeshComponent* Plane = Pair.Value)
                {
                    const FVector NewScale(
                        SlotData->PlaneSize.X / (EffectivePPU * 100.f),
                        SlotData->PlaneSize.Y / (EffectivePPU * 100.f),
                        1.f
                    );
                    Plane->SetRelativeScale3D(NewScale);
                }
            }
        }
    }

    UE_LOG(LogCharacterVisual, Log,
        TEXT("[Debug] PixelsPerUnit changed to %.2f — all planes rescaled."),
        PixelsPerUnit);
}
