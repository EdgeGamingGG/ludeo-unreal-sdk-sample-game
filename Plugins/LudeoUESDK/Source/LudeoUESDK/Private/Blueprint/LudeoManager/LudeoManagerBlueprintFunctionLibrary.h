#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LudeoManager/LudeoManagerTypes.h"
#include "LudeoResult.h"

#include "LudeoManagerBlueprintFunctionLibrary.generated.h"

UCLASS()
class ULudeoManagerBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Manager",
			DisplayName = "Initialize Ludeo SDK",
			WorldContext = "WorldContextObject"
		)
	)
	static FLudeoResult InitializeLudeoSDK(UObject* WorldContextObject);

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Manager",
			DisplayName = "Finalize Ludeo SDK",
			WorldContext = "WorldContextObject"
		)
	)
	static void FinalizeLudeoSDK(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (Category = "Ludeo Manager"))
	static void TickLudeoSDK();
};
