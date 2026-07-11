// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterMovementInput.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * Handles Enhanced Input bindings for a 2.5D platformer character.
 *
 * Responsibilities:
 *  - Adds the Input Mapping Context on BeginPlay.
 *  - Binds IA_Move  (Axis1D – left/right only) and IA_Jump.
 *  - Feeds the owning ACharacter's AddMovementInput / Jump / StopJumping.
 *
 * All InputAction / IMC references are UPROPERTY(EditDefaultsOnly) so they
 * are assigned in the Blueprint child class.  Everything is null-safe.
 */
UCLASS(ClassGroup = (Input), meta = (BlueprintSpawnableComponent))
class PLATFORMGAME_API UCharacterMovementInput : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterMovementInput();

	UFUNCTION(BlueprintPure, Category = "Input")
	float GetMoveInputValue() const { return MoveInputValue; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input",
			  meta = (DisplayName = "Input Mapping Context"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input",
			  meta = (DisplayName = "Mapping Context Priority"))
	int32 MappingContextPriority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input",
			  meta = (DisplayName = "Move Action"))
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input",
			  meta = (DisplayName = "Jump Action"))
	TObjectPtr<UInputAction> JumpAction;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input",
			  meta = (AllowPrivateAccess = "true",
			          DisplayName = "Move Input Value"))
	float MoveInputValue = 0.f;

	void AddMappingContext();
	void BindInputActions();

	void HandleMove(const FInputActionValue& Value);
	void HandleJumpStarted(const FInputActionValue& Value);
	void HandleJumpCompleted(const FInputActionValue& Value);
};
