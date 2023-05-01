// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
	isAiming = shooterCharacter->IsAiming();
	isCrouched = shooterCharacter->bIsCrouched;
	
	FRotator aimRotation = shooterCharacter->GetBaseAimRotation();
	FRotator movementRotation = UKismetMathLibrary::MakeRotFromX(shooterCharacter->GetVelocity());
    FRotator deltaRot = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation);

	deltaRotation = FMath::RInterpTo(deltaRotation, deltaRot, deltaTime, 15.f);

	YawOffset = deltaRotation.Yaw;

	charRotationLastFrame = charRotationThisFrame;
	charRotationThisFrame = shooterCharacter->GetActorRotation();

	const FRotator delta = UKismetMathLibrary::NormalizedDeltaRotator(charRotationThisFrame, charRotationLastFrame);
	const float target = delta.Yaw / deltaTime;
	const float interpolated = FMath::FInterpTo(Lean, target, deltaTime, 6.f);
	Lean = FMath::Clamp(interpolated, -90.f, 90.f);
}
 