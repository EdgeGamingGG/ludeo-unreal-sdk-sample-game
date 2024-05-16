#include "LudeoMainMenuGameMode.h"

#include "GameFramework/GameSession.h"

#include "LudeoGameInstance.h"

void ALudeoMainMenuGameMode::HandleMatchIsWaitingToStart()
{
	if (GameSession != nullptr)
	{
		GameSession->HandleMatchIsWaitingToStart();
	}

	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
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

bool ALudeoMainMenuGameMode::ReadyToStartMatch_Implementation()
{
	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	return !GameInstance->HasPendingSessionActivation();
}
