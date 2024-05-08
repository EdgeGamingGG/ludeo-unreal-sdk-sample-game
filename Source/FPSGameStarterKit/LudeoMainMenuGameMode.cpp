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

	if (GameInstance->GetActiveSessionHandle() == nullptr)
	{
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
	else
	{
		SessionSetupResult.Emplace(GameInstance->GetActiveSessionHandle());
	}
}

bool ALudeoMainMenuGameMode::ReadyToStartMatch_Implementation()
{
	return SessionSetupResult.IsSet();
}
