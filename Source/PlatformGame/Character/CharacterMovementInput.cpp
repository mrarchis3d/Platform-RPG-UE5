// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterMovementInput.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/Character.h"
#include "../PlatformGameLogs.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacterInput, Log, All);

UCharacterMovementInput::UCharacterMovementInput()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterMovementInput::BeginPlay()
{
	Super::BeginPlay();

	AddMappingContext();
	BindInputActions();
}

void UCharacterMovementInput::AddMappingContext()
{
	if (!DefaultMappingContext)
	{
		UE_LOG(LogCharacterInput, Warning,
			TEXT("[CharacterMovementInput] DefaultMappingContext is null – skipping."));
		return;
	}

	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		return;
	}

	const APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
	if (!PC)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (!Subsystem)
	{
		return;
	}

	Subsystem->AddMappingContext(DefaultMappingContext, MappingContextPriority);
}

// ─── Action Bindings 
void UCharacterMovementInput::BindInputActions()
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		return;
	}

	const APlayerController* PC = Cast<APlayerController>(PawnOwner->GetController());
	if (!PC)
	{
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PawnOwner->InputComponent);
	if (!EIC)
	{
		UE_LOG(LogCharacterInput, Warning,
			TEXT("[CharacterMovementInput] No EnhancedInputComponent found on owner."));
		return;
	}

	if (MoveAction)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this,
			&UCharacterMovementInput::HandleMove);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this,
			&UCharacterMovementInput::HandleMove);
	}
	else
	{
		UE_LOG(LogCharacterInput, Warning,
			TEXT("[CharacterMovementInput] MoveAction is null – movement not bound."));
	}

	if (JumpAction)
	{
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this,
			&UCharacterMovementInput::HandleJumpStarted);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this,
			&UCharacterMovementInput::HandleJumpCompleted);
	}
	else
	{
		UE_LOG(LogCharacterInput, Warning,
			TEXT("[CharacterMovementInput] JumpAction is null – jump not bound."));
	}
}

void UCharacterMovementInput::HandleMove(const FInputActionValue& Value)
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	MoveInputValue = Value.Get<float>();

	const FVector RightDirection = FVector::YAxisVector;
	CharacterOwner->AddMovementInput(RightDirection, MoveInputValue);
}

void UCharacterMovementInput::HandleJumpStarted(const FInputActionValue& /*Value*/)
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	CharacterOwner->Jump();
}

void UCharacterMovementInput::HandleJumpCompleted(const FInputActionValue& /*Value*/)
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	CharacterOwner->StopJumping();
}
