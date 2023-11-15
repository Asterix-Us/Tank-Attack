// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Characters/TaTankPawn.h"
#include "Components/CapsuleComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaTankPawn)

ATaTankPawn::ATaTankPawn()
{
	// Default Capsule settings
	CapsuleComp.Get()->SetCapsuleHalfHeight(59.f);
	CapsuleComp.Get()->SetCapsuleRadius(59.f);
	CapsuleComp->SetCollisionProfileName(FName("BlockAll"));
}
