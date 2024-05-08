#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LudeoUESDK/LudeoRoom/LudeoRoomTypes.h"

#include "LudeoRoomBlueprintFunctionLibrary.generated.h"

UCLASS()
class ULudeoRoomlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, meta = (Category = "Ludeo Room", DisplayName = "Is Valid Ludeo Room Handle"))
	static bool IsValidLudeoRoomHandle(const FLudeoRoomHandle& PlayerHandle);
};
