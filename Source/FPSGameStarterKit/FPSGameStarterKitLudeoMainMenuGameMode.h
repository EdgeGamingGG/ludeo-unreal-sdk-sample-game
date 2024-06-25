#pragma once

#include "GameFramework/GameMode.h"

#include "LudeoUESDK/LudeoSession/LudeoSessionTypes.h"

#include "FPSGameStarterKitLudeoMainMenuGameMode.generated.h"

UCLASS(config=Game)
class AFPSGameStarterKitLudeoMainMenuGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	virtual void HandleMatchIsWaitingToStart() override;
	virtual bool ReadyToStartMatch_Implementation() override;

private:
	TOptional<FLudeoSessionHandle> SessionSetupResult;
};
