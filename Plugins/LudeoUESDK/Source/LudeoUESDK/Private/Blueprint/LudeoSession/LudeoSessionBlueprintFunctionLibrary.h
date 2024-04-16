#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LudeoSession/LudeoSessionTypes.h"

#include "LudeoSessionBlueprintFunctionLibrary.generated.h"

UCLASS()
class ULudeoSessionBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (Category = "Ludeo Session", DisplayName = "Create Ludeo Session"))
	static FLudeoSessionHandle CreateLudeoSession(const FCreateLudeoSessionParameters& CreateLudeoSessionParameters);

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Session", DisplayName = "Is Valid Ludeo Session Handle"))
	static bool IsValidLudeoSessionHandle(const FLudeoSessionHandle& SessionHandle);
};
