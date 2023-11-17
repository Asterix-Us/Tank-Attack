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

UCLASS(Blueprintable)
class TANKATTACK_API ATaTankCharacter : public ACharacter, public ITaCombatInterface
{
	GENERATED_BODY()

public:
	ATaTankCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Tank", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> CameraComp;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|Tank", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Materials for both player and enemy tanks */
	UPROPERTY(EditAnywhere, Category = "Components|Tank")
	TObjectPtr<UMaterialInterface> PlayerTankMat;

	UPROPERTY(EditAnywhere, Category = "Components|Tank")
	TObjectPtr<UMaterialInterface> EnemyTankMat;

	UPROPERTY(VisibleAnywhere, Category = "Components|Tank")
	TObjectPtr<UTaCombatComponent> CombatComp;
	
	void UpdateMaterial();

public:
#pragma region TaCombatInterface
	// Blueprint alternative
	// void FireWeapon_Implementation() override;

	// C++ only version
	virtual void FireWeapon() override; 
#pragma endregion
	
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComp; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
