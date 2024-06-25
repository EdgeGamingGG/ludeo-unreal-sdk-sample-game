#include "FPSGameStarterKitLudeoGameMode.h"
#include "FPSGameStarterKitLudeoGameState.h"
#include "FPSGameStarterKitLudeoPlayerController.h"

#include "GameFramework/GameSession.h"

void AFPSGameStarterKitLudeoGameMode::HandleMatchIsWaitingToStart()
{
	if (GameSession != nullptr)
	{
		GameSession->HandleMatchIsWaitingToStart();
	}
}

bool AFPSGameStarterKitLudeoGameMode::ReadyToStartMatch_Implementation()
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
		if (AFPSGameStarterKitLudeoPlayerController* PlayerController = Cast<AFPSGameStarterKitLudeoPlayerController>(Itr->Get()))
		{
			bIsReadyToStartMatch = bIsReadyToStartMatch && PlayerController->IsPlayerReady();
		}
	}

	return bIsReadyToStartMatch;
}
