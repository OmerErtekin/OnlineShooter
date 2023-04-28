// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "OnlineShooter/Weapon/Weapon.h"
#include "OnlineShooter/Character/ShooterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::EquipWeapon(AWeapon* weaponToEquip)
{
	if (character == nullptr || weaponToEquip == nullptr) return;

	currentWeapon = weaponToEquip;
	currentWeapon->ShowPickupWidget(false);
	currentWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* handSocket = character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (handSocket)
	{
		handSocket->AttachActor(currentWeapon, character->GetMesh());
	}
	currentWeapon->SetOwner(character);
}

