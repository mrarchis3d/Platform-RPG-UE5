// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ECharacterSlots.h"
#include "CharacterBase.generated.h"

class UCharacterVisualComponent;
class UCharacterMovementInput;
class UTwoWayPlatformHandler;
class UBodyDataAsset;
class UEquipmentDataAsset;
class USkeletalMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class PLATFORMGAME_API ACharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    ACharacterBase();

    UFUNCTION(BlueprintCallable, Category = "Character",
              meta = (DisplayName = "Initialize Body"))
    void InitializeBody(UBodyDataAsset* BodyDataAsset);

    UFUNCTION(BlueprintCallable, Category = "Character|Equipment",
              meta = (DisplayName = "Equip"))
    void Equip(EEquipmentSlotType Slot, UEquipmentDataAsset* EquipmentData);

    UFUNCTION(BlueprintCallable, Category = "Character|Equipment", 
              meta = (DisplayName = "Unequip"))
    void Unequip(EEquipmentSlotType Slot);

    UFUNCTION(BlueprintCallable, Category = "Character|Equipment",
              meta = (DisplayName = "Clear Equipment"))
    void ClearEquipment();

    UFUNCTION(BlueprintPure, Category = "Character",
              meta = (DisplayName = "Get Visual Component"))
    UCharacterVisualComponent* GetVisualComponent() const
    {
        return VisualComponent;
    }

    UFUNCTION(BlueprintPure, Category = "Character|Input",
              meta = (DisplayName = "Get Movement Input Component"))
    UCharacterMovementInput* GetMovementInputComponent() const
    {
        return MovementInputComponent;
    }

    UFUNCTION(BlueprintPure, Category = "Character|Platform",
              meta = (DisplayName = "Get Two Way Platform Handler"))
    UTwoWayPlatformHandler* GetTwoWayPlatformHandler() const;

    UFUNCTION(BlueprintPure, Category = "Character|Equipment",
              meta = (DisplayName = "Is Slot Equipped"))
    bool IsSlotEquipped(EEquipmentSlotType Slot) const;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components",
              meta = (DisplayName = "Visual Component",
                      AllowPrivateAccess = "true"))
    TObjectPtr<UCharacterVisualComponent> VisualComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components",
              meta = (DisplayName = "Movement Input",
                      AllowPrivateAccess = "true"))
    TObjectPtr<UCharacterMovementInput> MovementInputComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components",
              meta = (DisplayName = "Two Way Platform Handler",
                      AllowPrivateAccess = "true"))
    TObjectPtr<UTwoWayPlatformHandler> TwoWayPlatformHandler;

    UPROPERTY(Transient)
    TMap<EEquipmentSlotType, TObjectPtr<UEquipmentDataAsset>> EquippedItems;
};
