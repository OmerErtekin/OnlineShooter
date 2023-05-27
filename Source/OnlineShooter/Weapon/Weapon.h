// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initilal UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};


UCLASS()
class ONLINESHOOTER_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool state);
	void Fire();

	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return weaponMesh; }

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);


private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		USkeletalMeshComponent* weaponMesh;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class USphereComponent* triggerAreaSphere;
	UPROPERTY(ReplicatedUsing = OnRep_weaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState weaponState;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class UWidgetComponent* pickupWidget;
	UFUNCTION()
		void OnRep_weaponState();
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		class UAnimationAsset* fireAnimation;
public:
	void SetWeaponState(EWeaponState state);
};
