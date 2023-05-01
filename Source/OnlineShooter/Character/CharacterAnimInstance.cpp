// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	shooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UCharacterAnimInstance::NativeUpdateAnimation(float deltaTime)
{
	Super::NativeUpdateAnimation(deltaTime);

	if (shooterCharacter == nullptr)
	{
		shooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (shooterCharacter == nullptr) return;

	FVector velocity = shooterCharacter->GetVelocity();
	velocity.Z = 0.f;
	speed = velocity.Size();
	isInAir = shooterCharacter->GetCharacterMovement()->IsFalling();
	isAccelerating = shooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0;
	isWeaponEquipped = shooterCharacter->IsWeaponEquipped();
	isCrouched = shooterCharacter->bIsCrouched;
}
