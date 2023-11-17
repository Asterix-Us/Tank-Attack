// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TaPlayerController.generated.h"

class UNiagaraSystem;
class UInputAction;
class UInputMappingContext;

UCLASS(DisplayName = "Ta Player Controller")
class TANKATTACK_API ATaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATaPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input")
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input")
	TObjectPtr<UNiagaraSystem> FXCursor;

	/** Mapping Context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SetDestinationClickAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShootAction;

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	void OnSetDestinationReleased();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveToLocation(const FVector_NetQuantize& DesiredLocation);

	void Fire();

private:
	UPROPERTY(Replicated)
	FVector CachedDestination;
	
	float FollowTime; // For how long it has been pressed
};
