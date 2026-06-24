// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECharacterSlots.generated.h"

UENUM(BlueprintType)
enum class EBodySlotType : uint8
{
    Head,
    Torso,
    Pelvis,

    LeftUpperArm,
    RightUpperArm,

    LeftLowerArm,
    RightLowerArm,

    LeftHand,
    RightHand,

    LeftThigh,
    RightThigh,

    LeftLeg,
    RightLeg,

    LeftFoot,
    RightFoot,

    Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
    Head,
    Armor,
    Weapon,
    Boots,
    Gloves,
    Collar,
    Ring,

    Max UMETA(Hidden)
};
