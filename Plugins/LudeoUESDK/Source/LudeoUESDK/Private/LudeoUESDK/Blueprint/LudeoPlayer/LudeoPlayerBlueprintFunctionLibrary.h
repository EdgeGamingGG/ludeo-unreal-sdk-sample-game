#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LudeoUESDK/LudeoPlayer/LudeoPlayerTypes.h"

#include "LudeoPlayerBlueprintFunctionLibrary.generated.h"

UCLASS()
class ULudeoPlayerlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Player", DisplayName = "Is Valid Ludeo Player Handle"))
	static bool IsValidLudeoPlayerHandle(const FLudeoPlayerHandle& PlayerHandle);
};
