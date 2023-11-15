// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Characters/TaTowerPawn.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaTowerPawn)

ATaTowerPawn::ATaTowerPawn()
{
	// Default Capsule settings
	CapsuleComp.Get()->SetCapsuleHalfHeight(62.f);
	CapsuleComp.Get()->SetCapsuleRadius(54.f);
}
