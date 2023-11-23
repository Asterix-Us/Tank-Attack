// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "UI/TaPlayerName.h"
#include "Components/EditableText.h"
#include "Types/SlateEnums.h"

void UTaPlayerName::NativeConstruct()
{
	Super::NativeConstruct();

	EditableNameText->OnTextCommitted.AddDynamic(this, &UTaPlayerName::HandleOnTextCommitted);
}

void UTaPlayerName::HandleOnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	NewPlayerName = Text.ToString();
}
