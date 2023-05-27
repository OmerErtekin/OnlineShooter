// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ONLINESHOOTER_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> projectileClass;
public:
	virtual void Fire(const FVector& hitTarget) override;
};
