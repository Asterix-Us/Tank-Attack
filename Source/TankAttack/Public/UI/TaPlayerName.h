// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaPlayerName.generated.h"

class UTextBlock;
class UEditableText;

UCLASS()
class TANKATTACK_API UTaPlayerName : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NamePromptText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WarningText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableText> EditableNameText;

	UPROPERTY()
	FString NewPlayerName;

private:
	UFUNCTION()
	void HandleOnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
};
