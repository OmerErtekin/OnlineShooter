// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

UCLASS()
class ONLINESHOOTER_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	void MoveForward(float value);
	void MoveRight(float value);
	void Turn(float value);
	void LookUp(float value);
	void EquipButtonPressed();

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* overheadWidget;
	UPROPERTY(ReplicatedUsing = OnRep_overlappingWeapon)
		class AWeapon* overlappingWeapon;
	UFUNCTION()
		void OnRep_overlappingWeapon(AWeapon* lastWeapon);
	UPROPERTY(VisibleAnywhere)
		class UCombatComponent* combat;
	UFUNCTION(Server,Reliable)
	void ServerEquipButtonPressed();
public:
	AShooterCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents();
	void SetOverlappingWeapon(AWeapon* weapon);
};
