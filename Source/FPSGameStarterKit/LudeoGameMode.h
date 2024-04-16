#pragma once

#include "GameFramework/GameMode.h"

#include "LudeoGameSession.h"

#include "LudeoGameMode.generated.h"

UCLASS(config=Game)
class ALudeoGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE ALudeoGameSession* GetGameSession() const
	{
		return Cast<ALudeoGameSession>(GameSession);
	}

private:
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void HandleMatchHasStarted() override;
	virtual bool ReadyToStartMatch_Implementation() override;
};
