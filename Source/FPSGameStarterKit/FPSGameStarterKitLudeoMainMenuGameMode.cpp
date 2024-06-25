#include "FPSGameStarterKitLudeoMainMenuGameMode.h"

#include "GameFramework/GameSession.h"

#include "FPSGameStarterKitLudeoGameInstance.h"

void AFPSGameStarterKitLudeoMainMenuGameMode::HandleMatchIsWaitingToStart()
{
	if (GameSession != nullptr)
	{
		GameSession->HandleMatchIsWaitingToStart();
	}

	UFPSGameStarterKitLudeoGameInstance* GameInstance = Cast<UFPSGameStarterKitLudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	if (!GameInstance->HasPendingSessionActivation())
	{
		const bool bHasActiveLudeoSession = (GameInstance->GetActiveLudeoSessionHandle(this) != nullptr);

		if (!bHasActiveLudeoSession)
		{
			GameInstance->SetupLudeoSession();
		}
		else
		{
			GameInstance->ReleasePendingLudeo();
		}
	}
}

bool AFPSGameStarterKitLudeoMainMenuGameMode::ReadyToStartMatch_Implementation()
{
	UFPSGameStarterKitLudeoGameInstance* GameInstance = Cast<UFPSGameStarterKitLudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	return !GameInstance->HasPendingSessionActivation();
}
