// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ONLINESHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	friend class AShooterCharacter;

	void EquipWeapon(class AWeapon* weaponToEquip);

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool state);

	UFUNCTION(Server, Reliable)
		void ServerSetAiming(bool state);
	UFUNCTION()
	void OnRep_EquipWeapon();
public:
	class AShooterCharacter* character;
	UPROPERTY(ReplicatedUsing = OnRep_EquipWeapon)
		class AWeapon* currentWeapon;
	UPROPERTY(Replicated)
	bool isAiming;
};
