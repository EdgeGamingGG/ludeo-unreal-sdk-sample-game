#include "LudeoUESDK/Blueprint/LudeoPlayer/LudeoPlayerBlueprintFunctionLibrary.h"

#include "LudeoUESDK/LudeoPlayer/LudeoPlayer.h"

bool ULudeoPlayerlueprintFunctionLibrary::IsValidLudeoPlayerHandle(const FLudeoPlayerHandle& PlayerHandle)
{
	return (FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle) != nullptr);
}
