// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Gameplay/Shooting/TaProjectile.h"
#include "Characters/TaTankCharacter.h"
#include "Components/BoxComponent.h"
#include "Controllers/TaPlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TankAttack/TankAttack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaProjectile)

ATaProjectile::ATaProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Create components
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Projectile Mesh");
	RootComponent = ProjectileMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	SetRootComponent(CollisionBox);
	CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	ParticleSystemComp = CreateDefaultSubobject<UParticleSystemComponent>("Particle System Comp");
	ParticleSystemComp->SetupAttachment(RootComponent);

	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Movement Comp");
	ProjectileMovementComp->bRotationFollowsVelocity = true;
	ProjectileMovementComp->SetIsReplicated(true);
	ProjectileMovementComp->InitialSpeed = 2000.f;
	ProjectileMovementComp->MaxSpeed = 2000.f;
}

void ATaProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind OnComponentHit delegate
	if (HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &ATaProjectile::OnHit);
	}

	// Play launch sound
	if (LaunchSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaunchSound, GetActorLocation());
	}
}

void ATaProjectile::Destroyed()
{
	Super::Destroyed();

	// Spawn hit particles
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticles, GetActorLocation(), GetActorRotation());
	}
	
	// Spawn hit sound
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}

void ATaProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	ATaTankCharacter* OwnerChar = Cast<ATaTankCharacter>(GetOwner());
	if (OwnerChar)
	{
		ATaPlayerController* OwnerController = Cast<ATaPlayerController>(OwnerChar->Controller);
		if (OwnerController)
		{
			if (OwnerChar->HasAuthority())
			{
				// Apply damage to the hit actor
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
				
				// Destroy the projectile
				Destroy();
				return;
			}
		}
	}
	Destroy();
}
