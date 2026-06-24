// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"
#include "CharacterVisualComponent.h"
#include "../PlatformGameLogs.h"
#include "Data/BodyDataAsset.h"
#include "Data/EquipmentDataAsset.h"
#include "Components/CapsuleComponent.h"


ACharacterBase::ACharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    VisualComponent = CreateDefaultSubobject<UCharacterVisualComponent>(TEXT("VisualComponent"));
    if (VisualComponent)
    {
        VisualComponent->SetupAttachment(GetRootComponent());
    }
}

void ACharacterBase::InitializeBody(UBodyDataAsset* BodyDataAsset)
{
    if (!BodyDataAsset)
    {
        UE_LOG(LogCharacterVisual, Warning, TEXT("[CharacterBase] InitializeBody called with null BodyDataAsset!"));
        return;
    }

    if (VisualComponent)
    {
        VisualComponent->SetTargetSkeleton(this->GetMesh());
        VisualComponent->InitializeBody(BodyDataAsset);
    }
}

void ACharacterBase::Equip(EEquipmentSlotType Slot, UEquipmentDataAsset* EquipmentData)
{
    if (Slot == EEquipmentSlotType::Max)
    {
        UE_LOG(LogCharacterVisual, Warning, TEXT("[CharacterBase] Equip: invalid slot Max!"));
        return;
    }

    if (!EquipmentData)
    {
        UE_LOG(LogCharacterVisual, Warning, TEXT("[CharacterBase] Equip: null EquipmentDataAsset for slot %s!"),
            *UEnum::GetValueAsString(Slot));
        return;
    }

    // If something is already equipped in this slot, unequip it first
    if (EquippedItems.Contains(Slot))
    {
        Unequip(Slot);
    }

    EquippedItems.Add(Slot, EquipmentData);

    if (VisualComponent)
    {
        VisualComponent->EquipVisual(Slot, EquipmentData);
    }
}

void ACharacterBase::Unequip(EEquipmentSlotType Slot)
{
    if (Slot == EEquipmentSlotType::Max)
    {
        return;
    }

    EquippedItems.Remove(Slot);

    if (VisualComponent)
    {
        VisualComponent->UnequipVisual(Slot);
    }
}

void ACharacterBase::ClearEquipment()
{
    EquippedItems.Empty();

    if (VisualComponent)
    {
        VisualComponent->ClearAllEquipmentVisuals();
    }
}

bool ACharacterBase::IsSlotEquipped(EEquipmentSlotType Slot) const
{
    return EquippedItems.Contains(Slot);
}
