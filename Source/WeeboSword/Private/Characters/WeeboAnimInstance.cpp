// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WeeboAnimInstance.h"
#include "Characters/WeeboCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
void UWeeboAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	WeeboCharacter = Cast<AWeeboCharacter>(TryGetPawnOwner());
	if (WeeboCharacter)
	{
		WeeboCharacterMovement = WeeboCharacter->GetCharacterMovement();
	}
}

void UWeeboAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (WeeboCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(WeeboCharacterMovement->Velocity);
		IsFalling = WeeboCharacterMovement->IsFalling();
		CharacterState = WeeboCharacter->GetCharacterState();
		ActionState = WeeboCharacter->GetActionState();
		DeathPose = WeeboCharacter->GetDeathPose();
	}
	
}
