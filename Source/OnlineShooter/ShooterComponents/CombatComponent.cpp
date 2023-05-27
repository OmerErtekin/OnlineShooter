// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "OnlineShooter/Weapon/Weapon.h"
#include "OnlineShooter/Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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

	if (currentWeapon == nullptr) return;

	if (character && isPressed)
	{
		character->PlayFireMontage(isAiming);
		currentWeapon->Fire();
	}
}



void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

