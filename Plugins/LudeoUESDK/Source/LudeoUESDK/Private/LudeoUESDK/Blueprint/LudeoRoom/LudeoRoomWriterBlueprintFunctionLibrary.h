#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "LudeoUESDK/LudeoRoom/LudeoRoomWriterTypes.h"

#include "LudeoRoomWriterBlueprintFunctionLibrary.generated.h"

UCLASS()
class ULudeoRoomWriterBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (Category = "Ludeo Data Writer", DisplayName = "Ludeo Data Writer Set Send Settings"))
	static bool SetSendSettings
	(
		const FLudeoRoomHandle& RoomHandle,
		const FLudeoRoomWriterSetSendSettingsParameters& SetSendSettingsParameters
	);

	UFUNCTION(BlueprintCallable, meta = (Category = "Ludeo Object", DisplayName = "Ludeo Room Writer Send Action"))
	static bool SendAction
	(
		const FLudeoRoomHandle& RoomHandle,
		const FLudeoRoomWriterSendActionParameters& SendActionParameters
	);
};
