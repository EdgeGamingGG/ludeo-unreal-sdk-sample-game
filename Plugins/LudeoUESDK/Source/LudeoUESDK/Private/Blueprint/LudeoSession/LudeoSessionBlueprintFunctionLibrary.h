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
	static FLudeoSessionHandle CreateLudeoSession();

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Session", DisplayName = "Ludeo Session Open Gallery"))
	static bool LudeoSessionOpenGallery(const FLudeoSessionHandle& SessionHandle);

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Session", DisplayName = "Is Valid Ludeo Session Handle"))
	static bool IsValidLudeoSessionHandle(const FLudeoSessionHandle& SessionHandle);
};
