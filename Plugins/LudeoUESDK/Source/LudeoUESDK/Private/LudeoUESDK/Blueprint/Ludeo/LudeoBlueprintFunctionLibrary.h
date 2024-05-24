#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LudeoUESDK/Ludeo/LudeoTypes.h"
#include "LudeoUESDK/LudeoSession/LudeoSessionTypes.h"

#include "LudeoBlueprintFunctionLibrary.generated.h"

UCLASS()
class ULudeoBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo", DisplayName = "Get Ludeo ID"))
	static FString GetLudeoID(const FLudeoHandle& LudeoHandle);
	
	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo", DisplayName = "Get Creator Player ID"))
	static FString GetCreatorPlayerID(const FLudeoHandle& LudeoHandle);

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo", DisplayName = "Is Valid Ludeo Handle"))
	static bool IsValidLudeoHandle(const FLudeoHandle& LudeoHandle);

	UFUNCTION
	(
		BlueprintPure,
		meta =
		(
			Category = "Ludeo",
			DisplayName = "Get Ludeo Game Window Handle",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static FLudeoGameWindowHandle GetGameWindowHandle(const UObject* WorldContextObject);
};
