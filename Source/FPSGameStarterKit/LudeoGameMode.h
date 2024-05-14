#pragma once

#include "GameFramework/GameMode.h"

#include "LudeoGameMode.generated.h"

UCLASS(config=Game)
class ALudeoGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	virtual void HandleMatchIsWaitingToStart() override;
	virtual bool ReadyToStartMatch_Implementation() override;
};
