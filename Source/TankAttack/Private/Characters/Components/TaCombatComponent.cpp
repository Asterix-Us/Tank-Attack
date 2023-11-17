// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Characters/Components/TaCombatComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Characters/TaTankCharacter.h"
#include "Gameplay/Shooting/TaProjectile.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaCombatComponent)

UTaCombatComponent::UTaCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTaCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTaCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UTaCombatComponent::FireButtonPressed()
{
	// Check if you can fire a new projectile
	if (bCanFire)
	{
		bCanFire = false;

		if (Character)
		{
			if (!Character->HasAuthority())
			{
				LocalFire();
			}
			ServerFire();
		}

		// Start a timer to wait until you can fire again
		StartFireTimer();
	}
}

void UTaCombatComponent::StartFireTimer()
{
	if (Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&ThisClass::FireTimerFinished,
		FireWaitTime);
}

void UTaCombatComponent::FireTimerFinished()
{
	bCanFire = true;
}

void UTaCombatComponent::LocalFire()
{
	// Get muzzle socket
	const USkeletalMeshSocket* MuzzleSocket = Character->GetMesh()->GetSocketByName(FName("SOCKET_Muzzle"));
	if (MuzzleSocket)
	{
		// Find socket location and set spawn params
		const FTransform SocketTransform = MuzzleSocket->GetSocketTransform(Character->GetMesh());
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;
		
		if (Character->HasAuthority())
		{
			// Spawn a new projectile on the server
			GetWorld()->SpawnActor<ATaProjectile>(
				ProjectileClass, SocketTransform.GetLocation(), Character->GetActorRotation(), SpawnParams);

			UE_LOG(LogTemp, Warning, TEXT("Projectile spawned!"));
		}
	}
}

void UTaCombatComponent::MulticastFire_Implementation()
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority())
		return;
	
	LocalFire();
}

void UTaCombatComponent::ServerFire_Implementation()
{
	MulticastFire();
}

bool UTaCombatComponent::ServerFire_Validate()
{
	return true;
}
