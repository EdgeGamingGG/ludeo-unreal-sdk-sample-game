#pragma once

#include "GameFramework/GameMode.h"

#include "LudeoUESDK/LudeoSession/LudeoSessionTypes.h"

#include "LudeoMainMenuGameMode.generated.h"

UCLASS(config=Game)
class ALudeoMainMenuGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	virtual void HandleMatchIsWaitingToStart() override;
	virtual bool ReadyToStartMatch_Implementation() override;

private:
	TOptional<FLudeoSessionHandle> SessionSetupResult;
};
