// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Characters/TaTankCharacter.h"
#include "Characters/PlayerStates/TaPlayerState.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Characters/Components/TaCombatComponent.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameModes/TaGameMode.h"
#include "GameStates/TaGameState.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaTankCharacter)

ATaTankCharacter::ATaTankCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(59.f, 59.f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create combat component
	CombatComp = CreateDefaultSubobject<UTaCombatComponent>(TEXT("CombatComp"));
	CombatComp->SetIsReplicated(true);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATaTankCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set event to receive damage notify
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}

	// Update material on the Tank, to make SimulatedProxies another color
	UpdateMaterial();
}

void ATaTankCharacter::PollInit()
{
	if (PlayerStateRef == nullptr)
	{
		PlayerStateRef = GetPlayerState<ATaPlayerState>();
		if (PlayerStateRef)
		{
			// Update player state values on the HUD
			OnPlayerStateInitialized();

			// Test if this player is currently a top scoring player
			ATaGameState* BlasterGameState = Cast<ATaGameState>(UGameplayStatics::GetGameState(this));
			if (BlasterGameState && BlasterGameState->TopScoringPlayers.Contains(PlayerStateRef))
			{
				MulticastGainedTheLead();
			}
		}
	}
}

void ATaTankCharacter::OnPlayerStateInitialized()
{
	// Update score and defeats on the HUD
	PlayerStateRef->AddToScore(0.f);
	PlayerStateRef->AddToDefeats(0);
}

void ATaTankCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                     AController* InstigatorController, AActor* DamageCauser)
{
	GameModeRef = GameModeRef == nullptr ? GetWorld()->GetAuthGameMode<ATaGameMode>() : GameModeRef;
	if (GameModeRef == nullptr) return;

	// Let the game mode know that the player was eliminated
	GameModeRef->PlayerEliminated(this, Controller, InstigatorController);
}

void ATaTankCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->SetCharacterRef(this);
	}
}

void ATaTankCharacter::Elim()
{
	MulticastElim();
}

void ATaTankCharacter::MulticastElim_Implementation()
{
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Cosmetics
	if (DeathParticles)
		UGameplayStatics::SpawnEmitterAtLocation(this, DeathParticles, GetActorLocation(), GetActorRotation());

	if (DeathSound)
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());

	// Destroy crown FX if this player was the top scoring player
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	
	// Respawn the player
	GameModeRef = GameModeRef == nullptr ? GetWorld()->GetAuthGameMode<ATaGameMode>() : GameModeRef;
	if (GameModeRef)
	{
		GameModeRef->HandleRespawn(this, Controller);
	}
}

void ATaTankCharacter::MulticastGainedTheLead_Implementation()
{
	// Spawn the crown FX on this player, since it's the top scoring player
	if (CrownSystem == nullptr) return;
	if (CrownComponent == nullptr)
	{
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	if (CrownComponent)
	{
		CrownComponent->Activate();
	}
}

void ATaTankCharacter::MulticastLostTheLead_Implementation()
{
	// Destroy the crown FX if the lead is lost
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
}

void ATaTankCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PollInit();
}

void ATaTankCharacter::UpdateMaterial()
{
	if (HasAuthority())
		return;
	
	if (IsLocallyControlled())
	{
		// Apply Player Material to the local player
		GetMesh()->SetMaterial(0, PlayerTankMat);
	}
	else
	{
		// Apply Enemy Material to other players
		GetMesh()->SetMaterial(0, EnemyTankMat);
	}
}

void ATaTankCharacter::FireWeapon()
{
	// Send the weapon firing input to the Combat Component
	if (CombatComp)
	{
		CombatComp->FireButtonPressed();
	}
}
