// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TaTankCharacter.generated.h"

class UCameraComponent;
class UMaterialInterface;
class USpringArmComponent;

UCLASS(Blueprintable)
class TANKATTACK_API ATaTankCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATaTankCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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
	
	void UpdateMaterial();

public:
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetCameraComponent() const { return CameraComp; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};
