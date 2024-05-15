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
		if (GameInstance->GetActiveLudeoSessionHandle(this) == nullptr)
		{
			GameInstance->SetupLudeoSession();
		}
	}
}

bool ALudeoMainMenuGameMode::ReadyToStartMatch_Implementation()
{
	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	return !GameInstance->HasPendingSessionActivation();
}
