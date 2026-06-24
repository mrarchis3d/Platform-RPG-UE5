// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ECharacterSlots.h"
#include "CharacterBase.generated.h"

class UCharacterVisualComponent;
class UBodyDataAsset;
class UEquipmentDataAsset;
class USkeletalMeshComponent;

UCLASS(BlueprintType, Blueprintable)
class PLATFORMGAME_API ACharacterBase : public ACharacter
{
    GENERATED_BODY()

public:
    ACharacterBase();

    /** Assigns the skeleton and builds body-part planes from the DataAsset. */
    UFUNCTION(BlueprintCallable, Category = "Character",
              meta = (DisplayName = "Initialize Body"))
    void InitializeBody(UBodyDataAsset* BodyDataAsset);

    /** Equips a piece of equipment in the given slot. Pass the DataAsset directly. */
    UFUNCTION(BlueprintCallable, Category = "Character|Equipment",
              meta = (DisplayName = "Equip"))
    void Equip(EEquipmentSlotType Slot, UEquipmentDataAsset* EquipmentData);

    /** Removes the equipment from the given slot. */
    UFUNCTION(BlueprintCallable, Category = "Character|Equipment",
              meta = (DisplayName = "Unequip"))
    void Unequip(EEquipmentSlotType Slot);

    /** Removes all equipped items. */
    UFUNCTION(BlueprintCallable, Category = "Character|Equipment",
              meta = (DisplayName = "Clear Equipment"))
    void ClearEquipment();

    UFUNCTION(BlueprintPure, Category = "Character",
              meta = (DisplayName = "Get Visual Component"))
    UCharacterVisualComponent* GetVisualComponent() const
    {
        return VisualComponent;
    }

    UFUNCTION(BlueprintPure, Category = "Character|Equipment",
              meta = (DisplayName = "Is Slot Equipped"))
    bool IsSlotEquipped(EEquipmentSlotType Slot) const;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Components",
              meta = (DisplayName = "Visual Component",
                      AllowPrivateAccess = "true"))
    TObjectPtr<UCharacterVisualComponent> VisualComponent;

    /** Local map of currently equipped items (slot -> DataAsset). */
    UPROPERTY(Transient)
    TMap<EEquipmentSlotType, TObjectPtr<UEquipmentDataAsset>> EquippedItems;
};
