#pragma once

#include "CoreMinimal.h"
#include "../ECharacterSlots.h"
#include "../CharacterSlotStructs.h"
#include "Engine/DataAsset.h"

#include "BodyDataAsset.generated.h"

UCLASS(BlueprintType)
class PLATFORMGAME_API UBodyDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body",
              meta = (DisplayName = "Body ID"))
    FName BodyID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Body",
              meta = (DisplayName = "Display Name"))
    FString DisplayName = TEXT("Base Character");
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual",
              meta = (DisplayName = "Base Material Template"))
    TObjectPtr<UMaterialInterface> BaseMaterialTemplate = nullptr;
	
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual",
              meta = (DisplayName = "Body Slots",
                      TitleProperty = "SlotType"))
    TMap<EBodySlotType, FBodySlotData> BodySlots;
};
