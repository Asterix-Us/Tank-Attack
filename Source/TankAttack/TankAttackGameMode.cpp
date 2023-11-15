// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "TankAttackGameMode.h"
#include "TankAttackPlayerController.h"
#include "TankAttackCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATankAttackGameMode::ATankAttackGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATankAttackPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Game/Core/Characters/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/Game/Core/Characters/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}