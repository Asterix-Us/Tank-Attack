// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TaProjectile.generated.h"

class USoundBase;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UParticleSystem;
class UStaticMeshComponent;
class UCameraShakeBase;
class UBoxComponent;

UCLASS()
class TANKATTACK_API ATaProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATaProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

private:
#pragma region Components
	UPROPERTY(VisibleAnywhere, Category = "Ta Projectile|Components")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Ta Projectile|Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComp;

	UPROPERTY(VisibleAnywhere, Category = "Ta Projectile|Components")
	TObjectPtr<UParticleSystemComponent> ParticleSystemComp;

	UPROPERTY(VisibleAnywhere, Category = "Ta Projectile|Components")
	TObjectPtr<UBoxComponent> CollisionBox;
	
#pragma endregion

#pragma region Parameters
	UPROPERTY(EditAnywhere, Category = "Ta Projectile|Combat Params")
	float Damage = 100.f;

	UPROPERTY(EditAnywhere, Category = "Ta Projectile|Combat Params")
	TObjectPtr<UParticleSystem> HitParticles;

	UPROPERTY(EditAnywhere, Category = "Ta Projectile|Combat Params")
	TObjectPtr<USoundBase> LaunchSound;

	UPROPERTY(EditAnywhere, Category = "Ta Projectile|Combat Params")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "Ta Projectile|Combat Params")
	TSubclassOf<UCameraShakeBase> HitCameraShake;
#pragma endregion

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
