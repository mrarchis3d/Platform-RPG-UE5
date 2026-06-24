// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterSlotStructs.generated.h"

class UMaterialInterface;


USTRUCT(BlueprintType)
struct FCharacterSlotConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot Config")
    FName SocketName = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot Config")
    FVector RelativeLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot Config")
    FRotator RelativeRotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot Config",
              meta = (DisplayName = "Plane Size in Pixels (X=Width, Y=Height)"))
    FVector2D PlaneSize = FVector2D(100.f, 100.f);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Slot Config",
              meta = (DisplayName = "ZOrder (higher = on top)"))
    int32 ZOrder = 0;
};


USTRUCT(BlueprintType)
struct FBodySlotData : public FCharacterSlotConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body Slot")
    EBodySlotType SlotType = EBodySlotType::Max;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body Slot",
              meta = (DisplayName = "Material Template"))
    TObjectPtr<UMaterialInterface> MaterialTemplate = nullptr;
};


USTRUCT(BlueprintType)
struct FEquipmentSlotData : public FCharacterSlotConfig
{
    GENERATED_BODY()

    /** Body slots que ESTE equipo oculta cuando se equipa.
     *  Ej: una armadura completa oculta Torso y Collar. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Slot",
              meta = (DisplayName = "Body Slots Hidden by this Equipment"))
    TArray<EBodySlotType> HiddenBodySlots;
};

