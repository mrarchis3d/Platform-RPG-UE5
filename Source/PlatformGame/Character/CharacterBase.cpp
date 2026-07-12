// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"
#include "CharacterVisualComponent.h"
#include "CharacterMovementInput.h"
#include "../PlatformGameLogs.h"
#include "Data/BodyDataAsset.h"
#include "Data/EquipmentDataAsset.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ACharacterBase::ACharacterBase()
{
    PrimaryActorTick.bCanEverTick = false;

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    if (UCharacterMovementComponent* CMC = GetCharacterMovement())
    {
        CMC->MaxAcceleration = 999999.f;
        CMC->BrakingDecelerationWalking = 999999.f;
        CMC->GroundFriction = 999999.f;
        CMC->BrakingFrictionFactor = 1.f;
        CMC->bUseSeparateBrakingFriction = false;

        CMC->MaxWalkSpeed = 120.f;

        CMC->JumpZVelocity = 800.f;
        CMC->AirControl = 0.8f;
        CMC->FallingLateralFriction = 0.f;

        CMC->SetPlaneConstraintEnabled(true);
        CMC->SetPlaneConstraintAxisSetting(EPlaneConstraintAxisSetting::X);
    }

    VisualComponent = CreateDefaultSubobject<UCharacterVisualComponent>(TEXT("VisualComponent"));
    if (VisualComponent)
    {
        VisualComponent->SetupAttachment(GetRootComponent());
    }

    MovementInputComponent = CreateDefaultSubobject<UCharacterMovementInput>(TEXT("MovementInput"));
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
