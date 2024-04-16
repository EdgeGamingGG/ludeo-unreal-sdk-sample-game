#include "LudeoGameMode.h"

#include "GameFramework/GameSession.h"

void ALudeoGameMode::HandleMatchIsWaitingToStart()
{
	if (GameSession != nullptr)
	{
		GameSession->HandleMatchIsWaitingToStart();
	}
}

void ALudeoGameMode::HandleMatchHasStarted()
{
	// start human players first
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (PlayerController && (PlayerController->GetPawn() == nullptr) && PlayerCanRestart(PlayerController))
		{
			RestartPlayer(PlayerController);
		}
	}

	Super::HandleMatchHasStarted();
}

bool ALudeoGameMode::ReadyToStartMatch_Implementation()
{
	if (ALudeoGameSession* LudeoGameSession = Cast<ALudeoGameSession>(GameSession))
	{
		return LudeoGameSession->IsSessionReady();
	}
	
	return false;
}
