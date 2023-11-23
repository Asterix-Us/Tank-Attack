// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TaScoreboardUtils.generated.h"

USTRUCT(BlueprintType)
struct FTaScoreboardInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	float PlayerScore;

	FTaScoreboardInfo()
	{
		PlayerName = FString();
		PlayerScore = 0.f;
	}
	
	FTaScoreboardInfo(FString Name, float Score)
	{
		PlayerName = Name;
		PlayerScore = Score;
	}
};
