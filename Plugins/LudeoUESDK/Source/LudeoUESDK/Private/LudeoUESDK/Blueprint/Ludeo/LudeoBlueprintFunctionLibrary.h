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
	UFUNCTION(BlueprintCallable, meta = (Category = "Ludeo", DisplayName = "Ludeo Get Ludeo ID"))
	static FString LudeoGetLudeoID(const FLudeoHandle& LudeoHandle);

	UFUNCTION(BlueprintCallable, meta = (Category = "Ludeo", DisplayName = "Ludeo Get Creator Player ID"))
	static FString LudeoGetCreatorPlayerID(const FLudeoHandle& LudeoHandle);

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo", DisplayName = "Is Valid Ludeo Handle"))
	static bool IsValidLudeoHandle(const FLudeoHandle& LudeoHandle);

	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo", DisplayName = "Get Ludeo Game Window Handle"))
	static FLudeoGameWindowHandle GetGameWindowHandle();
};
