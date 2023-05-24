#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "OnlineShooter/Weapon/Weapon.h"
#include "OnlineShooter/ShooterComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"


AShooterCharacter::AShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	overheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidet"));
	overheadWidget->SetupAttachment(RootComponent);

	combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	combat->character = this;
}


void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AShooterCharacter, overlappingWeapon, COND_OwnerOnly);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimOffset(DeltaTime);
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &AShooterCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AShooterCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AShooterCharacter::CrouchButtonReleased);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AShooterCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AShooterCharacter::AimButtonRelased);
	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &AShooterCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AShooterCharacter::LookUp);
}



void AShooterCharacter::MoveForward(float value)
{
	if (Controller == nullptr || value == 0) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
	AddMovementInput(Direction, value);
}

void AShooterCharacter::MoveRight(float value)
{
	if (Controller == nullptr || value == 0) return;

	const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
	const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
	AddMovementInput(Direction, value);
}

void AShooterCharacter::Turn(float value)
{
	AddControllerYawInput(value);
}

void AShooterCharacter::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void AShooterCharacter::AimButtonPressed()
{
	if (combat)
	{
		combat->SetAiming(true);
	}
}

void AShooterCharacter::AimButtonRelased()
{
	if (combat)
	{
		combat->SetAiming(false);
	}
}

void AShooterCharacter::EquipButtonPressed()
{
	if (combat)
	{
		if (HasAuthority())
		{
			combat->EquipWeapon(overlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}

}

void AShooterCharacter::ServerEquipButtonPressed_Implementation()
{
	if (combat)
	{
		combat->EquipWeapon(overlappingWeapon);
	}
}


void AShooterCharacter::CrouchButtonPressed()
{
	if (!bIsCrouched)
		Crouch();
}

void AShooterCharacter::CrouchButtonReleased()
{
	if (bIsCrouched)
		UnCrouch();
}

void AShooterCharacter::AimOffset(float DeltaTime)
{
	if (combat && combat->currentWeapon == nullptr) return;

	FVector velocity = GetVelocity();
	velocity.Z = 0.f;
	float speed = velocity.Size();

	bool isInAir = GetCharacterMovement()->IsFalling();
	if (speed == 0.f && !isInAir)
	{
		FRotator currentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator deltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentAimRotation, startAimRotation);
		AO_Yaw = deltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;
	}

	if (speed > 0.f || isInAir) //run or jump
	{
		startAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
	}


	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		FVector2D inRange(270.f, 360.f);
		FVector2D outRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(inRange, outRange, AO_Pitch);
	}
}


void AShooterCharacter::SetOverlappingWeapon(AWeapon* weapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(false);
	}
	overlappingWeapon = weapon;
	if (IsLocallyControlled() && overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(true);
	}
}

void AShooterCharacter::OnRep_overlappingWeapon(AWeapon* lastWeapon)
{
	if (overlappingWeapon)
	{
		overlappingWeapon->ShowPickupWidget(true);
	}
	if (lastWeapon)
	{
		lastWeapon->ShowPickupWidget(false);
	}
}

bool AShooterCharacter::IsWeaponEquipped()
{
	return combat && combat->currentWeapon;
}

bool AShooterCharacter::IsAiming()
{
	return combat && combat->isAiming;
}
