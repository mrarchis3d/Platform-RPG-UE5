// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterPlayerController.generated.h"

class UInputMappingContext;

UCLASS(BlueprintType, Blueprintable)
class PLATFORMGAME_API ACharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACharacterPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input",
			  meta = (DisplayName = "Default Mapping Context"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input",
			  meta = (DisplayName = "Mapping Context Priority"))
	int32 MappingContextPriority = 0;

private:
	void AddMappingContext();
};
