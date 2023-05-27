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

	void FireButtonPressed(bool isPressed);
	UFUNCTION(Server, Reliable)
		void ServerFire(const FVector_NetQuantize& traceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& traceHitTarget);

	void TraceUnderCrosshairs(FHitResult& traceHitResult);
public:

	class AShooterCharacter* character;
	UPROPERTY(ReplicatedUsing = OnRep_EquipWeapon)
		class AWeapon* currentWeapon;
	UPROPERTY(Replicated)
		bool isAiming;

	float baseWalkSpeed = 600.f;
	float aimWalkSpeed = 400.f;
	bool isFireButtonPressed;
};
