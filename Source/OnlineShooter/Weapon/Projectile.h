// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class ONLINESHOOTER_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit);
public:
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

private:

	UPROPERTY(EditAnywhere)
		class UBoxComponent* collisionBox;
	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* projectileMovementComponent;
	UPROPERTY(EditAnywhere)
		class UParticleSystem* tracer;

	class UParticleSystemComponent* tracerComponent;

	UPROPERTY(EditAnywhere)
		UParticleSystem* impactParticles;
	UPROPERTY(EditAnywhere)
		class USoundCue* impactSound;
};
