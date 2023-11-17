// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Controllers/TaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Gameplay/Shooting/TaCombatInterface.h"
#include "Gameplay/Shooting/TaProjectile.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaPlayerController)

ATaPlayerController::ATaPlayerController()
{
	// Default values
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	ShortPressThreshold = 0.3f;
	FollowTime = 0.f;
}

void ATaPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATaPlayerController, CachedDestination);
}

void ATaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ATaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ThisClass::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ThisClass::OnSetDestinationReleased);

		// Setup shoot input event
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ThisClass::Fire);
	}
}

void ATaPlayerController::OnSetDestinationReleased()
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;

	// If we hit a surface, cache the location
	if (GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		CachedDestination = Hit.Location;
	}

	// Stop any performing movements
	StopMovement();

	// Call MoveToLocation on server
	Server_MoveToLocation(CachedDestination);

	if (!HasAuthority())
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		
		// Spawn cursor particles locally
		if (IsLocalController())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				FXCursor,
				CachedDestination,
				FRotator::ZeroRotator,
				FVector(1.f),
				true,
				true,
				ENCPoolMethod::None,
				true);
		}
	}
}

void ATaPlayerController::Fire()
{
	// Call interface FireWeapon from the controlled Pawn
	
	// 1st interface version
	// if (GetPawn()->GetClass()->ImplementsInterface(UTaCombatInterface::StaticClass()))
	// {
	// 	Cast<ITaCombatInterface>(GetPawn())->FireWeapon(); // C++ only
	//  ITaCombatInterface::Execute_FireWeapon(GetPawn()); // Blueprint version
	// }

	// 2nd interface version
	ITaCombatInterface* CombatInterface = Cast<ITaCombatInterface>(GetPawn());
	if (CombatInterface)
	{
		CombatInterface->FireWeapon(); // C++ only
		// CombatInterface->Execute_FireWeapon(GetPawn()); // Blueprint version
	}
}

void ATaPlayerController::Server_MoveToLocation_Implementation(const FVector_NetQuantize& DesiredLocation)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DesiredLocation);
}

bool ATaPlayerController::Server_MoveToLocation_Validate(const FVector_NetQuantize& DesiredLocation)
{
	return true;
}
