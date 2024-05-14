#include "LudeoGameMode.h"
#include "LudeoGameState.h"
#include "LudeoPlayerController.h"

#include "GameFramework/GameSession.h"

void ALudeoGameMode::HandleMatchIsWaitingToStart()
{
	if (GameSession != nullptr)
	{
		GameSession->HandleMatchIsWaitingToStart();
	}
}

bool ALudeoGameMode::ReadyToStartMatch_Implementation()
{
	UWorld* World = GetWorld();
	check(World != nullptr)

	bool bIsReadyToStartMatch = Super::ReadyToStartMatch_Implementation();

	for (
		FConstPlayerControllerIterator Itr = World->GetPlayerControllerIterator();
		Itr&& bIsReadyToStartMatch;
		++Itr
	)
	{
		if (ALudeoPlayerController* PlayerController = Cast<ALudeoPlayerController>(Itr->Get()))
		{
			bIsReadyToStartMatch = bIsReadyToStartMatch && PlayerController->IsPlayerReady();
		}
	}

	return bIsReadyToStartMatch;
}
