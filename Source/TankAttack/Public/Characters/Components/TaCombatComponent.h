// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TaCombatComponent.generated.h"

class ATaProjectile;
class UParticleSystem;
class USoundBase;
class UCameraShakeBase;
class ATaTankCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKATTACK_API UTaCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTaCombatComponent();
	friend class ABlasterCharacter;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void FireButtonPressed();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Ta Combat")
	TSubclassOf<ATaProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Ta Combat|Cosmetic")
	TObjectPtr<UParticleSystem> DeathParticles;

	UPROPERTY(EditAnywhere, Category = "Ta Combat|Cosmetic")
	TObjectPtr<USoundBase> DeathSound;

	UPROPERTY(EditAnywhere, Category = "Ta Combat|Cosmetic")
	TSubclassOf<UCameraShakeBase> DeathCameraShake;
	
	UPROPERTY(EditAnywhere, Category = "Ta Combat")
	float FireWaitTime = 1.f;
	
	FTimerHandle FireTimer;
	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

	void LocalFire();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire();

	UPROPERTY()
	TObjectPtr<ATaTankCharacter> Character;

public:
	FORCEINLINE void SetCharacterRef(ATaTankCharacter* NewCharacter) { Character = NewCharacter; }
};
