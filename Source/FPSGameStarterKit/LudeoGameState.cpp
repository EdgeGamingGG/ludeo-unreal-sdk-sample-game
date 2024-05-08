#include "LudeoGameState.h"

#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Character.h"

#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "AIController.h"

#include "LudeoUESDK/LudeoManager/LudeoManager.h"
#include "LudeoUESDK/LudeoScopedGuard.h"

#include "LudeoGameInstance.h"
#include "LudeoPlayerController.h"

ALudeoGameState::ALudeoGameState() :
	Super(),
	LudeoGameSessionInitializationState(ELudeoGameSessionInitializationState::NotInitialized)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	bAllowTickBeforeBeginPlay = false;

	FLudeoSaveGameActorData& SaveGameActorData = SaveGameSpecification.SaveGameActorDataCollection.AddDefaulted_GetRef();
	SaveGameActorData.ActorFilter.MatchingActorClass = ALevelScriptActor::StaticClass();
	SaveGameActorData.Strategy = ELudeoSaveGameStrategy::Reconcile;
}

void ALudeoGameState::BeginPlay()
{
	Super::BeginPlay();

	if(LudeoGameSessionInitializationState == ELudeoGameSessionInitializationState::Succeeded)
	{
		// For player that starts from the beginning
		BeginGamePlay();
	}
}

void ALudeoGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority())
	{
		TickSaveObjectState();
	}
}

void ALudeoGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (LudeoGameSessionInitializationState == ELudeoGameSessionInitializationState::Succeeded)
	{
		EndGamePlay();

		CloseRoom();
	}
}

bool ALudeoGameState::IsLudeoGame(const UObject* WorldContextObject)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	if (const ALudeoGameState* GameState = World->GetGameState<ALudeoGameState>())
	{
		return GameState->IsLudeoGame();
	}

	return false;
}

bool ALudeoGameState::ReportLocalPlayerAction(const UObject* WorldContextObject, const ELudeoPlayerAction PlayerAction)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	if (const ALudeoGameState* GameState = World->GetGameState<ALudeoGameState>())
	{
		return GameState->ReportPlayerAction(GameState->GetLocalPlayerState(), PlayerAction);
	}

	return false;
}

bool ALudeoGameState::ReportPlayerAction(const APlayerState* PlayerState, const ELudeoPlayerAction PlayerAction) const
{
	check(PlayerState != nullptr);

	static UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TNameOf<ELudeoPlayerAction>::GetName());
	check(Enum != nullptr);

	FLudeoResult Result;

	if (const FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(LudeoRoomHandle))
	{
		FLudeoRoomWriterSendActionParameters SendActionParameters;
		SendActionParameters.ActionName = Enum->GetNameStringByValue(static_cast<int64>(PlayerAction));

		if(IsLudeoGame())
		{
			SendActionParameters.PlayerID = LudeoCreatorPlayerID.GetValue();
		}
		else
		{
			SendActionParameters.PlayerID = PlayerState->GetUniqueId().ToString();
		}

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

	return Result.IsSuccessful();
}

bool ALudeoGameState::IsSessionReady() const
{
	return
	(
		LudeoGameSessionInitializationState == ELudeoGameSessionInitializationState::Succeeded ||
		LudeoGameSessionInitializationState == ELudeoGameSessionInitializationState::Failed
	);
}

void ALudeoGameState::OnSessionReady(const bool bIsSuccessful)
{
	if (bIsSuccessful)
	{
		if (HasBegunPlay())
		{
			// For player that starts from the middle of the match
			BeginGamePlay();
		}
	}
	else
	{
		ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
		check(GameInstance != nullptr);

		GameInstance->DestoryLudeoSession(FOnLudeoSessionDestroyedDelegate::CreateUObject(this, &ALudeoGameState::OnLudeoSessionDestroyed));

		LudeoSessionHandle = nullptr;
		LudeoRoomHandle = nullptr;
		LudeoPlayerHandle = nullptr;
	}

	LudeoGameSessionInitializationState = (bIsSuccessful ? ELudeoGameSessionInitializationState::Succeeded : ELudeoGameSessionInitializationState::Failed);
}

void ALudeoGameState::OnLudeoSessionActivated(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const bool)
{
	if(Result.IsSuccessful())
	{	
		LudeoGameSessionInitializationState = ELudeoGameSessionInitializationState::WaitingForRoomSetup;

		ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
		check(GameInstance != nullptr);

		LudeoSessionHandle = SessionHandle;

		if (const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(GameInstance->GetPendingLudeoHandle()))
		{
			OnLudeoSelected(*Ludeo);
		}
		else
		{
			if (HasAuthority())
			{
				OpenRoom(FGuid::NewGuid().ToString(), FString());
			}
			else
			{
				OnRep_LudeoRoomInformation();
			}
		}
	}
	else
	{
		OnSessionReady(false);
	}
}

void ALudeoGameState::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle)
{
	LudeoSessionHandle = nullptr;
	LudeoRoomHandle = nullptr;
	LudeoPlayerHandle = nullptr;
}

void ALudeoGameState::OnLudeoPlayerBeginGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& PlayerHandle)
{
	
}

void ALudeoGameState::OnLudeoPlayerAdded(const FLudeoResult& Result, const FLudeoRoomHandle&, const FLudeoPlayerHandle& PlayerHandle)
{
	if (Result.IsSuccessful())
	{
		LudeoPlayerHandle = PlayerHandle;

		if (LudeoRoomHandle != nullptr)
		{
			OnSessionReady(true);
		}
	}
	else
	{
		OnSessionReady(false);
	}
}

void ALudeoGameState::OnLudeoRoomOpened(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle)
{
	if (Result.IsSuccessful())
	{
		FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(RoomHandle);
		check(LudeoRoom != nullptr);

		if (HasAuthority())
		{
			ReplicatedLudeoRoomInformation.RoomInformation = LudeoRoom->GetRoomInformation();
			ReplicatedLudeoRoomInformation.OpenRoomResult = Result;
			ReplicatedLudeoRoomInformation.bIsRoomResultReady = true;
		}

		AddPlayer(*LudeoRoom);
	}
	else
	{
		if (HasAuthority())
		{
			ReplicatedLudeoRoomInformation.OpenRoomResult = Result;
			ReplicatedLudeoRoomInformation.bIsRoomResultReady = true;
		}

		OnSessionReady(false);
	}
}

void ALudeoGameState::OnLudeoSelected(const FLudeo& Ludeo)
{
	OpenRoom(FGuid::NewGuid().ToString(), Ludeo.GetLudeoID());
}

void ALudeoGameState::OnLudeoRoomReady(const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle)
{
	LudeoRoomHandle = RoomHandle;

	if (LudeoPlayerHandle != nullptr)
	{
		OnSessionReady(true);
	}
}

void ALudeoGameState::OnRep_LudeoRoomInformation()
{
	if (
		LudeoGameSessionInitializationState == ELudeoGameSessionInitializationState::WaitingForRoomSetup &&
		LudeoSessionHandle != nullptr
	)
	{
		if (ReplicatedLudeoRoomInformation.bIsRoomResultReady)
		{
			if (ReplicatedLudeoRoomInformation.OpenRoomResult.IsSuccessful())
			{
				check(!ReplicatedLudeoRoomInformation.RoomInformation.RoomID.IsEmpty());
				
				OpenRoom
				(
					ReplicatedLudeoRoomInformation.RoomInformation.RoomID,
					ReplicatedLudeoRoomInformation.RoomInformation.LudeoID
				);

			}
			else
			{
				OnSessionReady(false);
			}
		}
	}
}

const APlayerState* ALudeoGameState::GetLocalPlayerState() const
{
	if (UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator Itr = World->GetPlayerControllerIterator(); Itr; ++Itr)
		{
			if (APlayerController* PlayerController = Itr->Get())
			{
				if (PlayerController->IsLocalController())
				{
					return PlayerController->PlayerState;
				}
			}
		}
	}

	return nullptr;
}

void ALudeoGameState::OpenRoom(const FString& RoomID, const FString& LudeoID)
{
	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(LudeoSessionHandle))
	{
		LudeoGameSessionInitializationState = ELudeoGameSessionInitializationState::RoomSetupOnTheFly;

		Session->GetOnRoomReadyDelegate().AddUObject(this, &ALudeoGameState::OnLudeoRoomReady);

		FLudeoSessionOpenRoomParameters OpenRoomParameters;
		OpenRoomParameters.RoomID = RoomID;
		OpenRoomParameters.LudeoID = LudeoID;

		Session->OpenRoom
		(
			OpenRoomParameters,
			FLudeoSessionOnOpenRoomDelegate::CreateUObject(this, &ALudeoGameState::OnLudeoRoomOpened)
		);
	}
	else
	{
		OnSessionReady(false);
	}
}

void ALudeoGameState::CloseRoom()
{
	if(LudeoRoomHandle != nullptr)
	{
		if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(LudeoSessionHandle))
		{
			Session->GetOnRoomReadyDelegate().RemoveAll(this);

			Session->GetOnRoomReadyDelegate().AddUObject(this, &ALudeoGameState::OnLudeoRoomReady);

			FLudeoSessionCloseRoomParameters CloseRoomParameters;
			CloseRoomParameters.RoomHandle = LudeoRoomHandle;

			Session->CloseRoom(CloseRoomParameters);
		}
	}
}

void ALudeoGameState::AddPlayer(FLudeoRoom& LudeoRoom)
{
	const APlayerState* PlayerState = GetLocalPlayerState();
	check(PlayerState != nullptr);

	LudeoGameSessionInitializationState = ELudeoGameSessionInitializationState::PlayerSetupOnTheFly;

	FLudeoRoomAddPlayerParameters AddPlayerParameters;
	AddPlayerParameters.PlayerID = PlayerState->GetUniqueId().ToString();

	LudeoRoom.AddPlayer
	(
		AddPlayerParameters,
		FLudeoRoomOnAddPlayerDelegate::CreateUObject(this, &ALudeoGameState::OnLudeoPlayerAdded)
	);
}

void ALudeoGameState::BeginGamePlay()
{
	if(LudeoPlayerHandle != nullptr)
	{
		const FLudeoPlayer* LudeoPlayer = FLudeoPlayer::GetPlayerByPlayerHandle(LudeoPlayerHandle);
		check(LudeoPlayer != nullptr);

		const FLudeoPlayerBeginGameplayParameters BeginGameplayParameters;
		LudeoPlayer->BeginGameplay
		(
			BeginGameplayParameters,
			FLudeoPlayerOnBeginGameplayDelegate::CreateUObject(this, &ALudeoGameState::OnLudeoPlayerBeginGameplay)
		);
	}
}

void ALudeoGameState::EndGamePlay()
{
	if(const FLudeoPlayer* LudeoPlayer = FLudeoPlayer::GetPlayerByPlayerHandle(LudeoPlayerHandle))
	{
		FLudeoPlayerEndGameplayParameters EndGameplayParameters;
		EndGameplayParameters.bIsAbort = true;

		LudeoPlayer->EndGameplay(EndGameplayParameters);
	}
}

void ALudeoGameState::SetupSession()
{
	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(GameInstance->GetActiveSessionHandle()))
	{
		OnLudeoSessionActivated(FLudeoResult::Success(), *LudeoSession, false);
	}
	else
	{
		LudeoGameSessionInitializationState = ELudeoGameSessionInitializationState::SessionSetupOnTheFly;

		GameInstance->SetupLudeoSession
		(
			FLudeoSessionOnActivatedDelegate::CreateUObject(this, &ALudeoGameState::OnLudeoSessionActivated)
		);
	}
}

void ALudeoGameState::HandleMatchIsWaitingToStart()
{
	FCoreDelegates::GameStateClassChanged.Broadcast(GetClass()->GetName());

	SetupSession();
}

void ALudeoGameState::TickSaveObjectState()
{
	if (const FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(LudeoRoomHandle))
	{
		FLudeoObjectStateManager::TickSaveObjectState(this, LudeoCreatorPlayerID, *Room, SaveGameSpecification, ObjectMap);

		for (APlayerState* PlayerState : PlayerArray)
		{
			if(PlayerState != nullptr)
			{
				if (const FLudeoWritableObject* WritableObject = ObjectMap.Find(PlayerState))
				{
					const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoWritableObject> EnterObjectGuard(*WritableObject);

					WritableObject->WriteData(TEXT("PlayerID"), PlayerState->GetUniqueId().ToString());
				}
			}
		}
	}
}

void ALudeoGameState::ProcessLudeoData(const FLudeo& Ludeo)
{
	UWorld* World = GetWorld();
	check(World != nullptr);

	APlayerController* CreatorPlayerController = World->GetFirstPlayerController();
	check(CreatorPlayerController != nullptr);

	const TArray<FLudeoObjectInformation>& ObjectInformationCollection = Ludeo.GetLudeoObjectInformationCollection();

	// Step one: Create the class collection
	TArray<TSubclassOf<UObject>> ObjectClassCollection;
	{
		TMap<TStringView<TCHAR>, TSubclassOf<UObject>> ObjectTypeClassMap;

		for (const FLudeoObjectInformation& ObjectInformation : ObjectInformationCollection)
		{
			TSubclassOf<UObject>& ObjectClass = ObjectTypeClassMap.FindOrAdd(ObjectInformation.ObjectType.GetCharArray());

			if (ObjectClass == nullptr)
			{
				ObjectClass = LoadClass<UObject>(nullptr, *ObjectInformation.ObjectType);

				if (ObjectClass != nullptr && ObjectClass->IsChildOf(ALevelScriptActor::StaticClass()))
				{
					if(ALevelScriptActor* LevelScriptActor = World->GetLevelScriptActor())
					{
						ObjectClass = LevelScriptActor->GetClass();
					}
				}
			}
			check(ObjectClass != nullptr);

			ObjectClassCollection.Emplace(ObjectClass);
		}
	}
	check(ObjectInformationCollection.Num() == ObjectClassCollection.Num())

	// Step two: Find out the creator player state object handle
	const FLudeoObjectHandle CreatorPlayerStateObjectHandle = [&]()
	{
		for (int32 i = 0; i < ObjectInformationCollection.Num(); ++i)
		{
			const FLudeoReadableObject& ReadableObjecct = ObjectInformationCollection[i].ReadableObject;
			const TSubclassOf<UObject>& ObjectClass = ObjectClassCollection[i];

			if (ObjectClass->IsChildOf(APlayerState::StaticClass()))
			{
				const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObjecct);

				FString PlayerID;
				const bool bHasRead = ReadableObjecct.ReadData(TEXT("PlayerID"), PlayerID);
				check(bHasRead);

				if (PlayerID == Ludeo.GetCreatorPlayerID())
				{
					return static_cast<FLudeoObjectHandle>(ReadableObjecct);
				}
			}
		}

		return FLudeoObjectHandle();
	}();
	check(CreatorPlayerStateObjectHandle.IsValid());

	// Step three: Spawn non-creator player controller and build the initial object map
	FLudeoReadableObject::ReadableObjectMapType RestoreWorldObjectMap;
	{
		/*
			Create the mapping of player controller and player state
		*/
		for (int32 i = 0; i < ObjectInformationCollection.Num(); ++i)
		{
			const FLudeoReadableObject& ReadableObject = ObjectInformationCollection[i].ReadableObject;
			const TSubclassOf<UObject>& ObjectClass = ObjectClassCollection[i];

			if (ObjectClass->IsChildOf(APlayerController::StaticClass()))
			{
				check(CreatorPlayerController->IsA(ObjectClass));

				const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObject);

				FLudeoObjectHandle PlayerStateObjectHandle;
				const bool bHasRead = ReadableObject.ReadData(TEXT("PlayerState"), PlayerStateObjectHandle);
				check(bHasRead && PlayerStateObjectHandle.IsValid());

				const int32 PlayerStateIndex = ObjectInformationCollection.IndexOfByPredicate
				(
					[&](const FLudeoObjectInformation& ObjectInformation)
					{
						return (ObjectInformation.ReadableObject == PlayerStateObjectHandle);
					}
				);
				check(ObjectInformationCollection.IsValidIndex(PlayerStateIndex));

				if(ObjectInformationCollection.IsValidIndex(PlayerStateIndex))
				{
					const FLudeoObjectInformation& PlayerStateObjectInformation = ObjectInformationCollection[PlayerStateIndex];
		
					APlayerController* PlayerController = [&]()
					{
						if (PlayerStateObjectHandle == CreatorPlayerStateObjectHandle)
						{
							return CreatorPlayerController;
						}
						else
						{
							FActorSpawnParameters ActorSpawnParameters;
							ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

							return World->SpawnActor<APlayerController>(CreatorPlayerController->GetClass(), ActorSpawnParameters);
						}
					}();
					check(PlayerController != nullptr);
					check(PlayerController->PlayerState != nullptr);

					RestoreWorldObjectMap.Emplace(ReadableObject, PlayerController);
					RestoreWorldObjectMap.Emplace(PlayerStateObjectInformation.ReadableObject, PlayerController->PlayerState);
				}
			}
		}
	}

	// Step four: Restore the world using the object map
	{
		const bool bHasCreatedRestoreWorldObjectMap = FLudeoObjectStateManager::CreateRestoreWorldObjectMap
		(
			World,
			ObjectInformationCollection,
			ObjectClassCollection,
			SaveGameSpecification,
			RestoreWorldObjectMap
		);
		check(bHasCreatedRestoreWorldObjectMap);

		const bool bHasWorldRestored = FLudeoObjectStateManager::RestoreWorld
		(
			SaveGameSpecification,
			RestoreWorldObjectMap
		);
		check(bHasWorldRestored);
	}

	for (FActorIterator Itr(World, ALudeoPlayerController::StaticClass()); Itr; ++Itr)
	{
		if (ALudeoPlayerController* LudeoPlayerController = Cast<ALudeoPlayerController>(*Itr))
		{
			check(LudeoPlayerController->PlayerState != nullptr);

			if (ACharacter* CharacterPawn = LudeoPlayerController->GetCharacterPawn())
			{
				LudeoPlayerController->PlayerState->SetIsABot(LudeoPlayerController != World->GetFirstPlayerController());

				LudeoPlayerController->Possess(CharacterPawn);
			}
		}
	}
}

void ALudeoGameState::ConditionalLoadLudeo()
{
	ULudeoGameInstance* GameInstance = Cast<ULudeoGameInstance>(GetGameInstance());
	check(GameInstance != nullptr);

	if(const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(GameInstance->GetPendingLudeoHandle()))
	{
		LudeoCreatorPlayerID = Ludeo->GetCreatorPlayerID();

		ProcessLudeoData(*Ludeo);
		
		GameInstance->ReleasePendingLudeo();
	}
}

void ALudeoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALudeoGameState, ReplicatedLudeoRoomInformation);
}