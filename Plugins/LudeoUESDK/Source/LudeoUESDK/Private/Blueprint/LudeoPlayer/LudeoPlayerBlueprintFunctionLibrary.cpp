#include "Blueprint/LudeoPlayer/LudeoPlayerBlueprintFunctionLibrary.h"

#include "LudeoPlayer/LudeoPlayer.h"

bool ULudeoPlayerlueprintFunctionLibrary::IsValidLudeoPlayerHandle(const FLudeoPlayerHandle& PlayerHandle)
{
	return (FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle) != nullptr);
}
