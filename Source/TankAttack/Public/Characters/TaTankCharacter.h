// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gameplay/Shooting/TaCombatInterface.h"
#include "TaTankCharacter.generated.h"

class UTaCombatComponent;
class UCameraComponent;
class UMaterialInterface;
class USpringArmComponent;
class ATaGameMode;
class UNiagaraSystem;
class UNiagaraComponent;
class ATaPlayerState;

UCLASS(Blueprintable)
class TANKATTACK_API ATaTankCharacter : public ACharacter, public ITaCombatInterface
{
	GENERATED_BODY()

public:
	ATaTankCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void PostInitializeComponents() override;

	/** Score lead mechanic */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainedTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	/** Elimination functions */
	void Elim();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

protected:
	virtual void BeginPlay() override;

	// Poll for any relevant classes and initialize our HUD
	void PollInit();
	void OnPlayerStateInitialized();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	
private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ta Components|Tank", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComp;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ta Components|Tank", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Materials for both player and enemy tanks */
	UPROPERTY(EditAnywhere, Category = "Ta Components|Tank")
	TObjectPtr<UMaterialInterface> PlayerTankMat;

	UPROPERTY(EditAnywhere, Category = "Ta Components|Tank")
	TObjectPtr<UMaterialInterface> EnemyTankMat;

	UPROPERTY(VisibleAnywhere, Category = "Ta Components|Tank")
	TObjectPtr<UTaCombatComponent> CombatComp;

	UPROPERTY(EditAnywhere, Category = "Ta Components|Tank|Cosmetic")
	TObjectPtr<UNiagaraSystem> CrownSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> CrownComponent;
	
	UPROPERTY(EditAnywhere, Category = "Ta Combat|Cosmetic")
	TObjectPtr<UParticleSystem> DeathParticles;

	UPROPERTY(EditAnywhere, Category = "Ta Combat|Cosmetic")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, Category = "Ta Combat|Cosmetic")
	TSubclassOf<UCameraShakeBase> DeathCameraShake;

	UPROPERTY()
	TObjectPtr<ATaGameMode> GameModeRef;

	UPROPERTY()
	TObjectPtr<ATaPlayerState> PlayerStateRef;
	
	void UpdateMaterial();

public:
#pragma region TaCombatInterface
	// Blueprint alternative
	// void FireWeapon_Implementation() override;

	// C++ only version
	virtual void FireWeapon() override; 
#pragma endregion
};
