#include "LudeoMainMenuGameMode.h"

#include "LudeoGameInstance.h"

bool ALudeoMainMenuGameMode::IsLudeoSessionActivated() const
{
	return
	(
		SessionSetupResult.IsSet() &&
		SessionSetupResult.GetValue() != nullptr
	);
}

bool ALudeoMainMenuGameMode::OpenLudeoSessionGallery()
{
	if(SessionSetupResult.IsSet())
	{
		if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionSetupResult.GetValue()))
		{
			const FLudeoResult Result = Session->OpenGallery();

			return Result.IsSuccessful();
		}
	}

	return false;
}


void ALudeoMainMenuGameMode::HandleMatchIsWaitingToStart()
{
	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	GameInstance->DestoryLudeoSession();

	GameInstance->SetupLudeoSession
	(
		FLudeoSessionOnActivatedDelegate::CreateWeakLambda
		(
			this,
			[this]
			(
				const FLudeoResult& Result,
				const FLudeoSessionHandle& SessionHandle,
				const bool
			)
			{
				if(Result.IsSuccessful())
				{
					SessionSetupResult.Emplace(SessionHandle);
				}
				else
				{
					SessionSetupResult.Emplace(nullptr);
				}
			}
		)
	);
}

void ALudeoMainMenuGameMode::HandleMatchHasStarted()
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


bool ALudeoMainMenuGameMode::ReadyToStartMatch_Implementation()
{
	return SessionSetupResult.IsSet();
}
