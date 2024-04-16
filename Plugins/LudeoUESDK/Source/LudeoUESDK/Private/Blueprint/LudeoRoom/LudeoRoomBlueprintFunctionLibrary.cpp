#include "Blueprint/LudeoRoom/LudeoRoomBlueprintFunctionLibrary.h"

#include "LudeoRoom/LudeoRoom.h"

bool ULudeoRoomlueprintFunctionLibrary::IsValidLudeoRoomHandle(const FLudeoRoomHandle& RoomHandle)
{
	return (FLudeoRoom::GetRoomByRoomHandle(RoomHandle) != nullptr);
}
