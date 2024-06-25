#pragma once

#include "GameFramework/GameMode.h"

#include "FPSGameStarterKitLudeoGameMode.generated.h"

UCLASS(config=Game)
class AFPSGameStarterKitLudeoGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	virtual void HandleMatchIsWaitingToStart() override;
	virtual bool ReadyToStartMatch_Implementation() override;
};
