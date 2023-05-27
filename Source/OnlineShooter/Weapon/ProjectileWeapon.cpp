// Fill out your copyright notice in the Description page of Project Settings.

#include "Engine/SkeletalMeshSocket.h"
#include "ProjectileWeapon.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& hitTarget)
{
	Super::Fire(hitTarget);
	APawn* instigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* muzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	if (muzzleSocket)
	{
		FTransform socketTransform = muzzleSocket->GetSocketTransform(GetWeaponMesh());
		FVector target = hitTarget - socketTransform.GetLocation();
		FRotator targetRotation = target.Rotation();

		if (projectileClass && instigatorPawn)
		{
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = GetOwner();
			spawnParams.Instigator = instigatorPawn;
			UWorld* world = GetWorld();
			if (world)
			{
				world->SpawnActor<AProjectile>(
					projectileClass,
					socketTransform.GetLocation(),
					targetRotation,
					spawnParams
				);
			}
		}
	}
}
