// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

ACharacterPlayerController::ACharacterPlayerController()
{
	bShowMouseCursor = false;
}

void ACharacterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ACharacterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AddMappingContext();
}

void ACharacterPlayerController::AddMappingContext()
{
	if (!DefaultMappingContext)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!Subsystem)
	{
		return;
	}

	Subsystem->AddMappingContext(DefaultMappingContext, MappingContextPriority);
}
