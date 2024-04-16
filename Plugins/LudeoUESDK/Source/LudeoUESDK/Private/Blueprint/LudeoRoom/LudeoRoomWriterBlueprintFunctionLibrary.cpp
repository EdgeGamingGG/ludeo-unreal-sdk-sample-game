#include "Blueprint/LudeoRoom/LudeoRoomWriterBlueprintFunctionLibrary.h"

#include "LudeoRoom/LudeoRoom.h"
#include "LudeoRoom/LudeoRoomWriterTypes.h"

bool ULudeoRoomWriterBlueprintFunctionLibrary::LudeoRoomWriterSetSendSettings
(
	const FLudeoRoomHandle& RoomHandle,
	const FLudeoRoomWriterSetSendSettingsParameters& SetSendSettingsParameters
)
{
	if (const FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(RoomHandle))
	{
		const FLudeoResult Result = Room->GetRoomWriter().SetSendSettings(SetSendSettingsParameters);

		return Result.IsSuccessful();
	}

	return false;
}

bool ULudeoRoomWriterBlueprintFunctionLibrary::LudeoRoomWriterSendAction
(
	const FLudeoRoomHandle& RoomHandle,
	const FLudeoRoomWriterSendActionParameters& SendActionParameters
)
{
	if (const FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(RoomHandle))
	{
		const FLudeoResult Result = Room->GetRoomWriter().SendAction(SendActionParameters);

		return Result.IsSuccessful();
	}

	return false;
}
