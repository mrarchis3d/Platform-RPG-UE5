// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterMovementInput.h"

#include "EnhancedInputComponent.h"
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

	// Set initial rotation to match bIsFacingRight
	if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
	{
		const float InitialYaw = bIsFacingRight ? 90.f : -90.f;
		CharacterOwner->SetActorRotation(FRotator(0.f, InitialYaw, 0.f));
	}

	BindInputActions();
}

void UCharacterMovementInput::BindInputActions()
{
	const APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (!PawnOwner)
	{
		return;
	}

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PawnOwner->InputComponent);
	if (!EIC)
	{
		return;
	}

	if (MoveAction)
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this,
			&UCharacterMovementInput::HandleMove);
		EIC->BindAction(MoveAction, ETriggerEvent::Completed, this,
			&UCharacterMovementInput::HandleMove);
	}

	if (JumpAction)
	{
		EIC->BindAction(JumpAction, ETriggerEvent::Started, this,
			&UCharacterMovementInput::HandleJumpStarted);
		EIC->BindAction(JumpAction, ETriggerEvent::Completed, this,
			&UCharacterMovementInput::HandleJumpCompleted);
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

	UpdateFacingDirection();
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

void UCharacterMovementInput::UpdateFacingDirection()
{
	if (FMath::IsNearlyZero(MoveInputValue))
	{
		return;
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	const float GroundSpeed = CharacterOwner->GetVelocity().Size2D();
	if (GroundSpeed <= 10.f)
	{
		return;
	}

	const bool bWantsRight = MoveInputValue > 0.f;

	if (bWantsRight == bIsFacingRight)
	{
		return;
	}

	bIsFacingRight = bWantsRight;
	const float TargetYaw = bIsFacingRight ? 90.f : -90.f;
	CharacterOwner->SetActorRotation(FRotator(0.f, TargetYaw, 0.f));
}
