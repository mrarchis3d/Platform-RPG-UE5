#pragma once

#include "CoreMinimal.h"
#include "../ECharacterSlots.h"
#include "../CharacterSlotStructs.h"
#include "Engine/DataAsset.h"

#include "EquipmentDataAsset.generated.h"

UCLASS(BlueprintType)
class PLATFORMGAME_API UEquipmentDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment",
              meta = (DisplayName = "Equipment ID"))
    FName EquipmentID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment",
              meta = (DisplayName = "Slot Type"))
    EEquipmentSlotType SlotType = EEquipmentSlotType::Max;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual",
              meta = (DisplayName = "Slot Config"))
    FEquipmentSlotData SlotConfig;
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual",
              meta = (DisplayName = "Equipment Material Template"))
    TObjectPtr<UMaterialInterface> MaterialTemplate = nullptr;
};
