#include "LudeoUESDK/Blueprint/LudeoRoom/LudeoRoomBlueprintFunctionLibrary.h"

#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

bool ULudeoRoomlueprintFunctionLibrary::IsValidLudeoRoomHandle(const FLudeoRoomHandle& RoomHandle)
{
	return (FLudeoRoom::GetRoomByRoomHandle(RoomHandle) != nullptr);
}
