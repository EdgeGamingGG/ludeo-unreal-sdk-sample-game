#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LudeoResult.h"

#include "LudeoUESDKBlueprintFunctionLibrary.generated.h"

UCLASS()
class ULudeoUESDKBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Result"))
	static bool IsSuccessfulLudeoResult(const FLudeoResult& Result)
	{
		return Result.IsSuccessful();
	}

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Result"))
	static bool IsFailedLudeoResult(const FLudeoResult& Result)
	{
		return Result.IsFailed();
	}

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Result"))
	static FString LudeoResultToString(const FLudeoResult& Result)
	{
		return FString(Result.ToString());
	}
};
