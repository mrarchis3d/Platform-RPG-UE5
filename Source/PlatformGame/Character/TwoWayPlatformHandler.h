// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TwoWayPlatformHandler.generated.h"

class UCapsuleComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (Collision), meta = (BlueprintSpawnableComponent))
class PLATFORMGAME_API UTwoWayPlatformHandler : public UActorComponent
{
	GENERATED_BODY()

public:
	UTwoWayPlatformHandler();

	UFUNCTION(BlueprintCallable, Category = "Platform")
	void RequestDropThrough();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:

	// ===== Settings =====

	UPROPERTY(EditDefaultsOnly, Category = "Platform")
	float DropThroughDuration = 0.3f;

	UPROPERTY(EditDefaultsOnly, Category = "Platform")
	float UpwardTraceDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Platform")
	float DownwardTraceDistance = 80.f;

	// ===== State =====

	float DropThroughTimer = 0.f;

	bool bIsDropping = false;
	bool bIsPassingThrough = false;

	TWeakObjectPtr<AActor> PassThroughPlatform;

	float PassThroughTopZ = 0.f;
	float PassThroughBottomZ = 0.f;

	// ===== Helpers =====

	bool TraceTwoWayPlatform(
		const FVector& Start,
		const FVector& End,
		FHitResult& OutHit) const;

	bool FindPlatformBelow(
		UCapsuleComponent* Capsule,
		FHitResult& OutHit) const;

	void SetPlatformCollision(
		UCapsuleComponent* CapsuleComponent,
		bool bBlock);
};