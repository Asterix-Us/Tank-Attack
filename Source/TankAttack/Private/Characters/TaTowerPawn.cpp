// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Characters/TaTowerPawn.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaTowerPawn)

ATaTowerPawn::ATaTowerPawn()
{
	// Default Capsule settings
	CapsuleComp->InitCapsuleSize(54.f, 62.f);
}
