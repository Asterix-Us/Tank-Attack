// Copyright Vasile Bogdan, 2023. All Rights Reserved.


#include "Characters/TaTankCharacter.h"
#include "Camera/CameraComponent.h"
#include "Characters/Components/TaCombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/Shooting/TaProjectile.h"
#include "Engine/SkeletalMeshSocket.h"

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
	
	UpdateMaterial();
}

void ATaTankCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATaTankCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComp)
	{
		CombatComp->SetCharacterRef(this);
	}
}

void ATaTankCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
	if (CombatComp)
	{
		CombatComp->FireButtonPressed();
	}
}
