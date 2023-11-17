// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TaCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTaCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class TANKATTACK_API ITaCombatInterface
{
	GENERATED_BODY()

public:
	// Blueprint functionality alternative
	// UFUNCTION(BlueprintNativeEvent, Category = "Ta Combat Interface")
	// void FireWeapon();

	// C++ only version
	UFUNCTION()
	virtual void FireWeapon() = 0;
};
