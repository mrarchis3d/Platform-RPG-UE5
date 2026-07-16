// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterMovementInput.h"

#include "CharacterBase.h"
#include "TwoWayPlatformHandler.h"
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

	if (DownAction)
	{
		EIC->BindAction(DownAction, ETriggerEvent::Triggered, this,
			&UCharacterMovementInput::HandleDown);
		EIC->BindAction(DownAction, ETriggerEvent::Completed, this,
			&UCharacterMovementInput::HandleDown);
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
	ACharacterBase* CharacterOwner = Cast<ACharacterBase>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	if (bIsHoldingDown)
	{
		if (UTwoWayPlatformHandler* PlatformHandler = CharacterOwner->GetTwoWayPlatformHandler())
		{
			PlatformHandler->RequestDropThrough();
		}
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
	if (GroundSpeed <= 15.f)
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

void UCharacterMovementInput::HandleDown(const FInputActionValue& Value)
{
	bIsHoldingDown = Value.Get<bool>();
}
