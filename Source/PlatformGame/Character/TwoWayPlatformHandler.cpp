// Copyright Epic Games, Inc. All Rights Reserved.

#include "TwoWayPlatformHandler.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "../PlatformCollisionChannels.h"

UTwoWayPlatformHandler::UTwoWayPlatformHandler()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
}

void UTwoWayPlatformHandler::BeginPlay()
{
	Super::BeginPlay();
}

bool UTwoWayPlatformHandler::TraceTwoWayPlatform(
	const FVector& Start,
	const FVector& End,
	FHitResult& OutHit) const
{
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(PlatformCollision::TwoWayPlatform);

	return GetWorld()->LineTraceSingleByObjectType(
		OutHit,
		Start,
		End,
		ObjectParams,
		Params);
}

bool UTwoWayPlatformHandler::FindPlatformBelow(
	UCapsuleComponent* Capsule,
	FHitResult& OutHit) const
{
	if (!Capsule)
	{
		return false;
	}

	const FVector Location = Capsule->GetComponentLocation();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float FeetZ = Location.Z - HalfHeight;

	const FVector Start(Location.X, Location.Y, FeetZ + 20.f);
	const FVector End(Location.X, Location.Y, FeetZ - DownwardTraceDistance);

	const bool bHit = TraceTwoWayPlatform(Start, End, OutHit);

	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		bHit ? FColor::Blue : FColor::Orange,
		false,
		0.f,
		0,
		2.f);

	return bHit;
}

void UTwoWayPlatformHandler::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner)
	{
		return;
	}

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	UCharacterMovementComponent* Movement = CharacterOwner->GetCharacterMovement();

	if (!Capsule || !Movement)
	{
		return;
	}

	const FVector Location = Capsule->GetComponentLocation();
	const float HalfHeight = Capsule->GetScaledCapsuleHalfHeight();

	const float FeetZ = Location.Z - HalfHeight;
	const float HeadZ = Location.Z + HalfHeight;

	// =====================================================
	// DROP THROUGH
	// =====================================================

	if (bIsDropping)
	{
		DropThroughTimer -= DeltaTime;

		if (HeadZ <= PassThroughBottomZ || DropThroughTimer <= 0.f)
		{
			bIsDropping = false;
			PassThroughPlatform = nullptr;

			SetPlatformCollision(Capsule, true);
		}
		else
		{
			SetPlatformCollision(Capsule, false);
			return;
		}
	}

	// =====================================================
	// PASS THROUGH FROM BELOW
	// =====================================================

	if (bIsPassingThrough)
	{
		if (FeetZ >= PassThroughTopZ - 10.f)
		{
			bIsPassingThrough = false;
			PassThroughPlatform = nullptr;

			SetPlatformCollision(Capsule, true);
		}
		else
		{
			SetPlatformCollision(Capsule, false);
		}

		return;
	}

	// =====================================================
	// CHECK PLATFORM ABOVE
	// =====================================================

	const FVector TraceStart(Location.X, Location.Y, HeadZ);
	const FVector TraceEnd = TraceStart + FVector(0.f, 0.f, UpwardTraceDistance);

	FHitResult UpHit;

	const bool bHitAbove = TraceTwoWayPlatform(
		TraceStart,
		TraceEnd,
		UpHit);

	DrawDebugLine(
		GetWorld(),
		TraceStart,
		TraceEnd,
		bHitAbove ? FColor::Green : FColor::Red,
		false,
		0.f,
		0,
		2.f);

	if (bHitAbove && Movement->Velocity.Z > 0.f)
	{
		bIsPassingThrough = true;
		PassThroughPlatform = UpHit.GetActor();

		if (UPrimitiveComponent* HitComp = UpHit.GetComponent())
		{
			const FBoxSphereBounds Bounds = HitComp->Bounds;

			PassThroughTopZ =
				Bounds.Origin.Z + Bounds.BoxExtent.Z;
		}
		else
		{
			PassThroughTopZ = UpHit.ImpactPoint.Z;
		}

		SetPlatformCollision(Capsule, false);
		return;
	}

	// Normal state
	SetPlatformCollision(Capsule, true);
}

void UTwoWayPlatformHandler::RequestDropThrough()
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());

	if (!CharacterOwner)
	{
		return;
	}

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	UCharacterMovementComponent* Movement = CharacterOwner->GetCharacterMovement();

	if (!Capsule || !Movement)
	{
		return;
	}

	// No permitir si ya está cayendo
	if (Movement->IsFalling())
	{
		return;
	}

	FHitResult Hit;

	if (!FindPlatformBelow(Capsule, Hit))
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[TwoWayPlatform] No platform below."));
		return;
	}

	bIsDropping = true;
	bIsPassingThrough = false;

	PassThroughPlatform = Hit.GetActor();

	if (UPrimitiveComponent* HitComp = Hit.GetComponent())
	{
		const FBoxSphereBounds Bounds = HitComp->Bounds;

		PassThroughBottomZ =
			Bounds.Origin.Z - Bounds.BoxExtent.Z;
	}
	else
	{
		PassThroughBottomZ = Hit.ImpactPoint.Z;
	}

	DropThroughTimer = DropThroughDuration;

	UE_LOG(LogTemp, Log,
		TEXT("[TwoWayPlatform] DropThrough started."));

	SetPlatformCollision(Capsule, false);
}

void UTwoWayPlatformHandler::SetPlatformCollision(
	UCapsuleComponent* CapsuleComponent,
	bool bBlock)
{
	if (!CapsuleComponent)
	{
		return;
	}

	const ECollisionResponse Desired =
		bBlock ? ECR_Block : ECR_Ignore;

	if (CapsuleComponent->GetCollisionResponseToChannel(
		PlatformCollision::TwoWayPlatform) != Desired)
	{
		CapsuleComponent->SetCollisionResponseToChannel(
			PlatformCollision::TwoWayPlatform,
			Desired);
	}
}