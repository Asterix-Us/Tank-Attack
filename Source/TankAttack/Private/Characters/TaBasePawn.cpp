// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Characters/TaBasePawn.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaBasePawn)

ATaBasePawn::ATaBasePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create components
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>("Capsule Component");

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("Base Mesh");
	if (BaseMesh) BaseMesh->SetupAttachment(CapsuleComp);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("Turret Mesh");
	if (TurretMesh) TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>("Projectile Spawn Point");
	if (ProjectileSpawnPoint) ProjectileSpawnPoint->SetupAttachment(TurretMesh);

	RootComponent = CapsuleComp;
}

void ATaBasePawn::BeginPlay()
{
	Super::BeginPlay();
}

void ATaBasePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
