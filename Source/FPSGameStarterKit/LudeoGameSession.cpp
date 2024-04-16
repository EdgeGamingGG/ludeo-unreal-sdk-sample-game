#include "LudeoGameSession.h"

#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

#include "LudeoManager/LudeoManager.h"

#include "LudeoGameInstance.h"
#include "LudeoGameMode.h"

ALudeoGameSession::ALudeoGameSession() :
	Super(),
	bIsLudeoGameSessionReady(false),
	PendingNumberOfPlayerAdded(0),
	bIsReloadingLudeo(false)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ALudeoGameSession::Tick(float)
{
	if (bIsLudeoGameSessionReady)
	{
		if (FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(LudeoRoomHandle))
		{
			LudeoObjectStateManager.TickUpdateObjectState(*Room, LudeoPlayerMap, SaveGameSpecification);
		}
	}
}

void ALudeoGameSession::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(LudeoSessionHandle))
	{
		CloseRoom();
	}
}

FLudeoSessionHandle ALudeoGameSession::GetActiveLudeoSessionHandle() const
{
	return LudeoSessionHandle;
}

bool ALudeoGameSession::ReportPlayerAction(const UObject* WorldContextObject, const int32 PlayerIndex, const ELudeoPlayerAction PlayerAction)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	ALudeoGameMode* GameMode = Cast<ALudeoGameMode>(World->GetAuthGameMode());
	check(GameMode != nullptr);

	ALudeoGameSession* GameSession = GameMode->GetGameSession();
	check(GameSession != nullptr);

	return GameSession->InternalReportPlayerAction(PlayerIndex, PlayerAction);
}

bool ALudeoGameSession::InternalReportPlayerAction(const int32 PlayerIndex, const ELudeoPlayerAction PlayerAction)
{
	static UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TNameOf<ELudeoPlayerAction>::GetName());
	check(Enum != nullptr);

	FLudeoResult Result;

	if (FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(LudeoRoomHandle))
	{
		const FLudeoPlayer* LudeoPlayer = [&]()
		{
			const AGameStateBase* GameState = GetGameState();
			check(GameState != nullptr);

			if(GameState->PlayerArray.IsValidIndex(PlayerIndex))
			{
				if (const FLudeoPlayerHandle* LudeoPlayerHandle = LudeoPlayerMap.Find(GameState->PlayerArray[PlayerIndex]))
				{
					return FLudeoPlayer::GetPlayerByPlayerHandle(*LudeoPlayerHandle);
				}
			}

			return static_cast<const FLudeoPlayer*>(nullptr);
		}();

		if (LudeoPlayer != nullptr)
		{
			FLudeoRoomWriterSendActionParameters SendActionParameters;
			SendActionParameters.ActionName = Enum->GetNameStringByValue(static_cast<int64>(PlayerAction));
			SendActionParameters.PlayerHandle = *LudeoPlayer;

			Result = LudeoRoom->GetRoomWriter().SendAction(SendActionParameters);

			UKismetSystemLibrary::PrintString
			(
				this,
				*FString::Printf
				(
					TEXT("Player action %s is reported. Result: %s"),
					*SendActionParameters.ActionName,
					(Result.IsSuccessful() ? TEXT("successful") : TEXT("unsuccessful"))
				),
				true,
				true,
				FLinearColor(0.0, 0.66, 1.0),
				5.0f
			);
		}
	}

	return Result.IsSuccessful();
}

bool ALudeoGameSession::IsSessionReady() const
{
	return bIsLudeoGameSessionReady;
}

void ALudeoGameSession::PostLogin(APlayerController* NewPlayer)
{
	check(NewPlayer != nullptr);
	check(NewPlayer->PlayerState != nullptr);

	if (FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(LudeoRoomHandle))
	{
		const AGameStateBase* GameState = GetGameState();
		check(GameState != nullptr);

		const int32 PlayerIndex = GameState->PlayerArray.IndexOfByKey(NewPlayer->PlayerState);
		check(GameState->PlayerArray.IsValidIndex(PlayerIndex));

		FLudeoRoomAddPlayerParameters AddPlayerParameters;
		AddPlayerParameters.PlayerID = FString::FromInt(PlayerIndex);

		LudeoRoom->AddPlayer
		(
			AddPlayerParameters,
			FLudeoRoomOnAddPlayerDelegate::CreateUObject(this, &ALudeoGameSession::OnLudeoPlayerAdded)
		);
	}
}

void ALudeoGameSession::OnSessionReady(const bool bIsSuccessful)
{
	if (!bIsSuccessful)
	{
		ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
		check(GameInstance != nullptr);

		GameInstance->DestoryLudeoSession(FOnLudeoSessionDestroyedDelegate::CreateUObject(this, &ALudeoGameSession::OnLudeoSessionDestroyed));

	}
	
	bIsLudeoGameSessionReady = true;
}

void ALudeoGameSession::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle)
{
	LudeoSessionHandle = nullptr;
	LudeoRoomHandle = nullptr;
	LudeoPlayerMap.Empty();
}

void ALudeoGameSession::OnLudeoPlayerBeginGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& PlayerHandle)
{
	
}

void ALudeoGameSession::OnLudeoPlayerAdded(const FLudeoResult& Result, const FLudeoRoomHandle&, const FLudeoPlayerHandle& PlayerHandle)
{
	if (Result.IsSuccessful())
	{
		const FLudeoPlayer* Player = FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle);
		check(Player != nullptr);

		const AGameStateBase* GameState = GetGameState();
		check(GameState != nullptr);

		const FString& PlayerID = Player->GetPlayerID();

		for (int32 i = 0; i < GameState->PlayerArray.Num(); ++i)
		{
			if (PlayerID == FString::FromInt(i))
			{
				LudeoPlayerMap.Emplace(GameState->PlayerArray[i], PlayerHandle);

				break;
			}
		}
	}

	PendingNumberOfPlayerAdded = PendingNumberOfPlayerAdded - 1;

	if(PendingNumberOfPlayerAdded == 0)
	{
		if(LudeoRoomHandle != nullptr)
		{
			AllPlayerBeginGameplay();
		}
	}
}

void ALudeoGameSession::OnLudeoRoomOpened(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle)
{
	if (Result.IsSuccessful())
	{
		FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(RoomHandle);
		check(LudeoRoom != nullptr);

		const AGameStateBase* GameState = GetGameState();
		check(GameState != nullptr);

		check(GameState->PlayerArray.Num() > 0);

		for (int32 i = 0; i < GameState->PlayerArray.Num(); ++i)
		{
			AddPlayer(*LudeoRoom, i);
		}
	}
	else
	{
		OnSessionReady(false);
	}
}

void ALudeoGameSession::OnGetLudeo(const FLudeoResult& Result, const FLudeoSessionHandle&, const FLudeoHandle& InLudeoHandle)
{
	if (Result.IsSuccessful())
	{
		const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(InLudeoHandle);
		check(Ludeo != nullptr);

		if(Ludeo != nullptr)
		{
			LudeoHandle = InLudeoHandle;

			OpenRoom(Ludeo->GetLudeoID());
		}
		else
		{
			OnSessionReady(false);
		}
	}
	else
	{
		OnSessionReady(false);
	}
}

void ALudeoGameSession::OnLudeoSelected(const FLudeoSessionHandle& SessionHandle, const FString& LudeoID)
{
	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetLudeo
		(
			LudeoID,
			FLudeoSessionOnGetLudeoDelegate::CreateUObject(this, &ALudeoGameSession::OnGetLudeo)
		);

		GameInstance->SetPendingLudeoIDToLoad({});
	}
}

void ALudeoGameSession::OnPauseGameRequested(const FLudeoSessionHandle& SessionHandle)
{
	UGameplayStatics::SetGamePaused(this, true);
}

void ALudeoGameSession::OnResumeGameRequested(const FLudeoSessionHandle& SessionHandle)
{
	UGameplayStatics::SetGamePaused(this, false);
}

void ALudeoGameSession::OnGameBackToMainMenuRequested(const FLudeoSessionHandle& SessionHandle)
{
	UGameplayStatics::OpenLevel(this, TEXT("MainMenu"));
}

void ALudeoGameSession::OnLudeoRoomReady(const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle)
{
	LudeoRoomHandle = RoomHandle;

	if (PendingNumberOfPlayerAdded == 0)
	{
		AllPlayerBeginGameplay();
	}
}

bool ALudeoGameSession::IsInGame() const
{
	if (UWorld* World = GetWorld())
	{
		return (UGameplayStatics::GetCurrentLevelName(this) != TEXT("MainMenu"));
	}

	return false;
}

void ALudeoGameSession::OpenRoom(const FString& LudeoID)
{
	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(LudeoSessionHandle))
	{
		Session->GetOnPauseGameRequestedDelegate().AddUObject(this, &ALudeoGameSession::OnPauseGameRequested);
		Session->GetOnResumeGameRequestedDelegate().AddUObject(this, &ALudeoGameSession::OnResumeGameRequested);
		Session->GetOnGameBackToMenuRequestedDelegate().AddUObject(this, &ALudeoGameSession::OnGameBackToMainMenuRequested);
		Session->GetOnRoomReadyDelegate().AddUObject(this, &ALudeoGameSession::OnLudeoRoomReady);

		FLudeoSessionOpenRoomParameters OpenRoomParameters;
		OpenRoomParameters.LudeoID = LudeoID;

		Session->OpenRoom
		(
			OpenRoomParameters,
			FLudeoSessionOnOpenRoomDelegate::CreateUObject(this, &ALudeoGameSession::OnLudeoRoomOpened)
		);
	}
	else
	{
		OnSessionReady(false);
	}
}

void ALudeoGameSession::CloseRoom()
{
	if(LudeoRoomHandle != nullptr)
	{
		if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(LudeoSessionHandle))
		{
			Session->GetOnRoomReadyDelegate().RemoveAll(this);

			Session->GetOnRoomReadyDelegate().AddUObject(this, &ALudeoGameSession::OnLudeoRoomReady);

			FLudeoSessionCloseRoomParameters CloseRoomParameters;
			CloseRoomParameters.RoomHandle = LudeoRoomHandle;

			Session->CloseRoom(CloseRoomParameters);
		}
	}
}

void ALudeoGameSession::AddPlayer(FLudeoRoom& LudeoRoom, const int32 PlayerIndex)
{
	PendingNumberOfPlayerAdded = PendingNumberOfPlayerAdded + 1;

	FLudeoRoomAddPlayerParameters AddPlayerParameters;
	AddPlayerParameters.PlayerID = FString::FromInt(PlayerIndex);

	LudeoRoom.AddPlayer
	(
		AddPlayerParameters,
		FLudeoRoomOnAddPlayerDelegate::CreateUObject(this, &ALudeoGameSession::OnLudeoPlayerAdded)
	);
}

void ALudeoGameSession::AllPlayerBeginGameplay()
{
	for (FLudeoObjectStateManager::PlayerMapType::TConstIterator Itr = LudeoPlayerMap.CreateConstIterator(); Itr; ++Itr)
	{
		const FLudeoPlayer* LudeoPlayer = FLudeoPlayer::GetPlayerByPlayerHandle(Itr->Get<1>());
		check(LudeoPlayer != nullptr);

		const FLudeoPlayerBeginGameplayParameters BeginGameplayParameters;
		LudeoPlayer->BeginGameplay
		(
			BeginGameplayParameters,
			FLudeoPlayerOnBeginGameplayDelegate::CreateUObject(this, &ALudeoGameSession::OnLudeoPlayerBeginGameplay)
		);
	}

	OnSessionReady(true);
}

const AGameStateBase* ALudeoGameSession::GetGameState() const
{
	if (UWorld* World = GetWorld())
	{
		if (AGameModeBase* GameMode = World->GetAuthGameMode())
		{
			return GameMode->GetGameState<AGameStateBase>();
		}
	}

	return nullptr;
}

void ALudeoGameSession::HandleMatchIsWaitingToStart()
{
	Super::HandleMatchIsWaitingToStart();

	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(GameInstance->GetActiveSessionHandle()))
	{
		LudeoSessionHandle = *LudeoSession;

		if (!GameInstance->GetPendingLudeoIDToLoad().IsEmpty())
		{
			OnLudeoSelected(*LudeoSession, GameInstance->GetPendingLudeoIDToLoad());
		}
		else
		{
			OpenRoom(FString());
		}
	}
	else
	{
		OnSessionReady(false);
	}
}

void ALudeoGameSession::HandleMatchHasStarted()
{
	if(LudeoHandle != nullptr)
	{
		const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(LudeoHandle);
		check(Ludeo != nullptr);

		if(Ludeo != nullptr)
		{
			const bool bHasWorldRestored = LudeoObjectStateManager.RestoreWorld(*Ludeo, this, SaveGameSpecification);
			check(bHasWorldRestored);
		}
	}

	Super::HandleMatchHasStarted();
}
