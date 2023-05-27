// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "OnlineShooter/Weapon/Weapon.h"
#include "OnlineShooter/Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#define TRACE_LENGTH 80000.f
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();	
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = baseWalkSpeed;
	}
}

void UCombatComponent::SetAiming(bool state)
{
	isAiming = state;
	ServerSetAiming(state);
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = isAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool state)
{
	isAiming = state;
	if (character)
	{
		character->GetCharacterMovement()->MaxWalkSpeed = isAiming ? aimWalkSpeed : baseWalkSpeed;
	}
}


void UCombatComponent::OnRep_EquipWeapon()
{
	if (currentWeapon && character)
	{
		character->GetCharacterMovement()->bOrientRotationToMovement = false;
		character->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool isPressed)
{
	isFireButtonPressed = isPressed;
	if(isFireButtonPressed)
		ServerFire();
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& traceHitResult)
{
	FVector2D viewPortSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(viewPortSize);
	}

	FVector2D crossHairLocation(viewPortSize.X / 2.f, viewPortSize.Y / 2.f);
	FVector crossHairWorldPosition;
	FVector crossHairWorldDirection;
	bool isScreenedToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		crossHairLocation,crossHairWorldPosition,crossHairWorldDirection
	);

	if (isScreenedToWorld)
	{
		FVector start = crossHairWorldPosition;
		FVector end = start + crossHairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(traceHitResult, start, end, ECollisionChannel::ECC_Visibility);
		if (!traceHitResult.bBlockingHit)
		{
			traceHitResult.ImpactPoint = end;
			hitTarget = end;
		}
		else
		{
			hitTarget = traceHitResult.ImpactPoint;
			DrawDebugSphere(GetWorld(), traceHitResult.ImpactPoint, 15.f, 15, FColor::Red);
		}
		
	}

}

void UCombatComponent::MulticastFire_Implementation()
{
	if (currentWeapon == nullptr) return;

	if (character)
	{
		character->PlayFireMontage(isAiming);
		currentWeapon->Fire(hitTarget);
	}
}

void UCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}



void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FHitResult hitResult;
	TraceUnderCrosshairs(hitResult);
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, currentWeapon);
	DOREPLIFETIME(UCombatComponent, isAiming);

}

void UCombatComponent::EquipWeapon(AWeapon* weaponToEquip)
{
	if (character == nullptr || weaponToEquip == nullptr) return;

	currentWeapon = weaponToEquip;
	currentWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (handSocket)
	{
		handSocket->AttachActor(currentWeapon, character->GetMesh());
	}
	currentWeapon->SetOwner(character);
	character->GetCharacterMovement()->bOrientRotationToMovement = false;
	character->bUseControllerRotationYaw = true;
}

