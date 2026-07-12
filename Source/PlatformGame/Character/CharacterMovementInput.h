// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterMovementInput.generated.h"

class UInputAction;
struct FInputActionValue;

UCLASS(ClassGroup = (Input), meta = (BlueprintSpawnableComponent))
class PLATFORMGAME_API UCharacterMovementInput : public UActorComponent
{
	GENERATED_BODY()

public:
	UCharacterMovementInput();

	UFUNCTION(BlueprintPure, Category = "Input")
	float GetMoveInputValue() const { return MoveInputValue; }

	UFUNCTION(BlueprintPure, Category = "Input")
	bool IsFacingRight() const { return bIsFacingRight; }

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Input",
			  meta = (AllowPrivateAccess = "true",
			          DisplayName = "Is Facing Right"))
	bool bIsFacingRight = true;

	void BindInputActions();

	void HandleMove(const FInputActionValue& Value);
	void HandleJumpStarted(const FInputActionValue& Value);
	void HandleJumpCompleted(const FInputActionValue& Value);

	void UpdateFacingDirection();
};
