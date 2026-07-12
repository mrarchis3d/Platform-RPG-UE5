// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"

#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
	}

}

void UCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (MovementComponent)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	}
}


