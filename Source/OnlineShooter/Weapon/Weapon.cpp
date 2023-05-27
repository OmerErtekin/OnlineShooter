#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "OnlineShooter/Character/ShooterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	weaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(weaponMesh);
	weaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	weaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	weaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	triggerAreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerAreaSphere"));
	triggerAreaSphere->SetupAttachment(RootComponent);
	triggerAreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	triggerAreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	pickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	pickupWidget->SetupAttachment(RootComponent);

}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	if (pickupWidget)
	{
		pickupWidget->SetVisibility(false);
	}

	if (HasAuthority())
	{
		triggerAreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		triggerAreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);
		triggerAreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		triggerAreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, weaponState);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AShooterCharacter* shooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (shooterCharacter)
	{
		shooterCharacter->SetOverlappingWeapon(this);
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

	AShooterCharacter* shooterCharacter = Cast<AShooterCharacter>(OtherActor);

	if (shooterCharacter)
	{
		shooterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState state)
{
	weaponState = state;
	switch (weaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		triggerAreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AWeapon::OnRep_weaponState()
{
	switch (weaponState)
	{	
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		break;
	}
}

void AWeapon::ShowPickupWidget(bool state)
{
	if (!pickupWidget) return;
	pickupWidget->SetVisibility(state);
}

void AWeapon::Fire()
{
	if (fireAnimation)
	{
		weaponMesh->PlayAnimation(fireAnimation, false);
	}
}

