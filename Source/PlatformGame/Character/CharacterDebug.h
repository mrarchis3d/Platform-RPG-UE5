// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "ECharacterSlots.h"
#include "CharacterDebug.generated.h"

class UBodyDataAsset;
class UEquipmentDataAsset;

/**
 * Debug/preview character for animators.
 * Auto-initializes body cards + equipment from DataAssets assigned in the Details panel.
 * Use in Sequencer with Control Rig to preview animations with full card visuals.
 *
 * NOT for production — use ACharacterBase in-game (data comes from API).
 */
UCLASS(BlueprintType, Blueprintable)
class PLATFORMGAME_API ACharacterDebug : public ACharacterBase
{
	GENERATED_BODY()

public:
	ACharacterDebug();

	/** Body DataAsset to auto-initialize on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Preview",
	          meta = (DisplayName = "Preview Body Data"))
	TObjectPtr<UBodyDataAsset> PreviewBodyData;

	/** Equipment to auto-equip on BeginPlay. Assign one DataAsset per slot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug|Preview",
	          meta = (DisplayName = "Preview Equipment"))
	TMap<EEquipmentSlotType, TObjectPtr<UEquipmentDataAsset>> PreviewEquipment;

protected:
	virtual void BeginPlay() override;
};
