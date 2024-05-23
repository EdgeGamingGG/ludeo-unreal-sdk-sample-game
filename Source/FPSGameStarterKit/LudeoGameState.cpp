#include "LudeoGameState.h"

#include "LudeoUESDK/LudeoScopedGuard.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameSession.h"
#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystemTypes.h"

#include "LudeoGameInstance.h"
#include "LudeoPlayerController.h"

ALudeoGameState::ALudeoGameState() :
	Super(),
	LudeoGameSessionInitializationState(ELudeoGameSessionInitializationState::NotInitialized)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	
	bAllowTickBeforeBeginPlay = false;

	// By default save level script actor - Important for figuring out the level name and variables in the level script blueprint
	{
		FLudeoSaveGameActorData& SaveGameActorData = SaveGameSpecification.SaveGameActorDataCollection.AddDefaulted_GetRef();
		SaveGameActorData.ActorFilter.MatchingActorClass = ALevelScriptActor::StaticClass();
		SaveGameActorData.Strategy = ELudeoSaveGameStrategy::Reconcile;
	}

	// By default save player controller - Important for identifying players
	{
		FLudeoSaveGameActorData& SaveGameActorData = SaveGameSpecification.SaveGameActorDataCollection.AddDefaulted_GetRef();
		SaveGameActorData.ActorFilter.MatchingActorClass = APlayerController::StaticClass();
		SaveGameActorData.ActorFilter.ActorPropertyFilter.MatchingPropertyNameFilter.PropertyNameCollection.Add(TEXT("PlayerState"));
		SaveGameActorData.Strategy = ELudeoSaveGameStrategy::Reconcile;
	}

	// By default save player state - Important for identifying players
	{
		FLudeoSaveGameActorData& SaveGameActorData = SaveGameSpecification.SaveGameActorDataCollection.AddDefaulted_GetRef();
		SaveGameActorData.ActorFilter.MatchingActorClass = APlayerState::StaticClass();
		SaveGameActorData.ActorFilter.ActorPropertyFilter.MatchingPropertyNameFilter.PropertyNameCollection.Add(TEXT("PlayerId"));
		SaveGameActorData.Strategy = ELudeoSaveGameStrategy::Reconcile;
	}
}

void ALudeoGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(HasAuthority())
	{
		FGameModeEvents::GameModeLogoutEvent.AddUObject(this, &ALudeoGameState::OnPlayerLeft);
	}
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

		if(HasAuthority())
		{
			CloseRoom();
		}
	}

	if (HasAuthority())
	{
		FGameModeEvents::GameModeLogoutEvent.RemoveAll(this);
	}
}

void ALudeoGameState::OnPlayerLeft(class AGameModeBase*, class AController* Controller)
{
	check(HasAuthority());

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (!PlayerController->IsLocalController())
		{
			if (FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(LudeoRoomHandle))
			{
				if(const APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>())
				{
					RemovePlayer(*PlayerState, *LudeoRoom);
				}
			}
		}
	}
}

bool ALudeoGameState::IsLudeoGame(const UObject* WorldContextObject)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	const ALudeoGameState* GameState = World->GetGameState<ALudeoGameState>();
	check(GameState != nullptr);

	return !GameState->ReplicatedLudeoRoomInformation.RoomInformation.LudeoID.IsEmpty();
}

bool ALudeoGameState::ReportPlayerAction(const APlayerState* PlayerState, const ELudeoPlayerAction PlayerAction)
{
	check(PlayerState != nullptr);
	
	UWorld* World = PlayerState->GetWorld();
	check(World != nullptr);

	const ALudeoGameState* GameState = World->GetGameState<ALudeoGameState>();
	check(GameState != nullptr);

	static UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TNameOf<ELudeoPlayerAction>::GetName());
	check(Enum != nullptr);

	FLudeoResult Result;

	if (const FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(GameState->LudeoRoomHandle))
	{
		FLudeoRoomWriterSendActionParameters SendActionParameters;
		SendActionParameters.ActionName = Enum->GetNameStringByValue(static_cast<int64>(PlayerAction));
		SendActionParameters.PlayerID = FString::FromInt(PlayerState->GetPlayerId());

		Result = LudeoRoom->GetRoomWriter().SendAction(SendActionParameters);

		UKismetSystemLibrary::PrintString
		(
			World,
			*FString::Printf
			(
				TEXT("[Player - %s (ID: %d)] Player action %s is reported. Result: %s"),
				*PlayerState->GetPlayerName(),
				PlayerState->GetPlayerId(),
				*SendActionParameters.ActionName,
				ANSI_TO_TCHAR(Result.ToString().GetData())
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
				ConditionalOpenRoom();
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

		const APlayerState* PlayerState = ALudeoGameState::GetLocalPlayerState(this);
		check(PlayerState != nullptr);

		AddPlayer(*PlayerState, *LudeoRoom);
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
	ReplicatedLudeoRoomInformation.RoomInformation.LudeoID = Ludeo.GetLudeoID();

	LoadLudeo(Ludeo);

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

void ALudeoGameState::ConditionalOpenRoom()
{
	LudeoGameSessionInitializationState = ELudeoGameSessionInitializationState::RoomSetupOnTheFly;

	UWorld* World = GetWorld();
	check(World != nullptr);

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer
	(
		TimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [&]()
		{
			const bool bHasReplicatedPlayerState = [&]()
			{
				if (const APlayerState* LocalPlayerState = ALudeoGameState::GetLocalPlayerState(this))
				{
					return (LocalPlayerState->GetPlayerId() != LocalPlayerState->GetClass()->GetDefaultObject<APlayerState>()->GetPlayerId());
				}

				return false;
			}();
			
			if (bHasReplicatedPlayerState)
			{
				if(HasAuthority())
				{
					OpenRoom(FGuid::NewGuid().ToString(), FString());
				}
				else
				{
					check(!ReplicatedLudeoRoomInformation.RoomInformation.RoomID.IsEmpty());

					OpenRoom(ReplicatedLudeoRoomInformation.RoomInformation.RoomID, ReplicatedLudeoRoomInformation.RoomInformation.LudeoID);
				}
			}
			else
			{
				ConditionalOpenRoom();
			}
		}),
		0.5f,
		false
	);
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
				ConditionalOpenRoom();

			}
			else
			{
				OnSessionReady(false);
			}
		}
	}
}

const APlayerState* ALudeoGameState::GetLocalPlayerState(const UObject* WorldContextObject)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	if (const ALudeoGameState* GameState = World->GetGameState<ALudeoGameState>())
	{
		for (const APlayerState* PlayerState : GameState->PlayerArray)
		{
			if (PlayerState != nullptr)
			{
				if (APlayerController* PlayerController = Cast<APlayerController>(PlayerState->GetOwner()))
				{
					if (PlayerController->IsLocalController())
					{
						return PlayerController->PlayerState;
					}
				}
			}
		}
	}

	return nullptr;
}

const APlayerState* ALudeoGameState::GetObjectAssociatedPlayerState(const UObject* Object)
{
	return FLudeoObjectStateManager::GetObjectAssociatedPlayerState(Object);
}

void ALudeoGameState::OpenRoom(const FString& RoomID, const FString& LudeoID)
{
	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(LudeoSessionHandle))
	{
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

			FLudeoSessionCloseRoomParameters CloseRoomParameters;
			CloseRoomParameters.RoomHandle = LudeoRoomHandle;

			Session->CloseRoom(CloseRoomParameters);
		}
	}
}

void ALudeoGameState::AddPlayer(const APlayerState& PlayerState, FLudeoRoom& LudeoRoom)
{
	check(PlayerState.GetPlayerId() != PlayerState.GetClass()->GetDefaultObject<APlayerState>()->GetPlayerId());

	LudeoGameSessionInitializationState = ELudeoGameSessionInitializationState::PlayerSetupOnTheFly;

	FLudeoRoomAddPlayerParameters AddPlayerParameters;
	AddPlayerParameters.PlayerID = FString::FromInt(PlayerState.GetPlayerId());

	LudeoRoom.AddPlayer
	(
		AddPlayerParameters,
		FLudeoRoomOnAddPlayerDelegate::CreateUObject(this, &ALudeoGameState::OnLudeoPlayerAdded)
	);
}

void ALudeoGameState::RemovePlayer(const APlayerState& PlayerState, FLudeoRoom& LudeoRoom)
{
	FLudeoRoomRemovePlayerParameters RemovePlayerParameters;
	RemovePlayerParameters.PlayerID = FString::FromInt(PlayerState.GetPlayerId());

	LudeoRoom.RemovePlayer(RemovePlayerParameters);
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

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(GameInstance->GetActiveLudeoSessionHandle(this)))
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
		FLudeoObjectStateManager::SaveWorld(this, *Room, SaveGameSpecification, ObjectMap);
	}
}

void ALudeoGameState::LoadLudeo(const FLudeo& Ludeo)
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
	const FLudeoObjectHandle TargetPlayerStateObjectHandle = [&]()
	{
		FLudeoObjectHandle FallbackPlayerStateObjectHandle;

		for (int32 i = 0; i < ObjectInformationCollection.Num(); ++i)
		{
			const FLudeoReadableObject& ReadableObject = ObjectInformationCollection[i].ReadableObject;
			const TSubclassOf<UObject>& ObjectClass = ObjectClassCollection[i];

			if (ObjectClass->IsChildOf(APlayerState::StaticClass()))
			{
				const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObject);

				// Read PlayerId with either string due to the nature of FName being case-insensitive
				int32 PlayerID;
				const bool bHasRead = (ReadableObject.ReadData(TEXT("PlayerId"), PlayerID) || ReadableObject.ReadData(TEXT("PlayerID"), PlayerID));
				check(bHasRead);

				if (Ludeo.GetCreatorPlayerID() == FString::FromInt(PlayerID))
				{
					return static_cast<FLudeoObjectHandle>(ReadableObject);
				}
				else
				{
					FallbackPlayerStateObjectHandle = static_cast<FLudeoObjectHandle>(ReadableObject);
				}
			}
		}

		return FallbackPlayerStateObjectHandle;
	}();
	check(TargetPlayerStateObjectHandle.IsValid());

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
				check(CreatorPlayerController->GetClass() == ObjectClass);

				FLudeoObjectHandle PlayerStateObjectHandle;
				{
					const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObject);

					const bool bHasRead = ReadableObject.ReadData(TEXT("PlayerState"), PlayerStateObjectHandle);
					check(bHasRead && PlayerStateObjectHandle.IsValid());
				}

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
						if (PlayerStateObjectHandle == TargetPlayerStateObjectHandle)
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

	// Step four: Restore the world
	{
		const bool bHasWorldRestored = FLudeoObjectStateManager::RestoreWorld
		(
			World,
			ObjectInformationCollection,
			ObjectClassCollection,
			SaveGameSpecification,
			RestoreWorldObjectMap
		);
		check(bHasWorldRestored);

		for (FLudeoReadableObject::ReadableObjectMapType::TConstIterator Itr = RestoreWorldObjectMap.CreateConstIterator(); Itr; ++Itr)
		{
			if (USceneComponent* SceneComponent = Cast<USceneComponent>(Itr->Get<1>()))
			{
				if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(SceneComponent))
				{
					if (PrimitiveComponent->IsSimulatingPhysics())
					{
						PrimitiveComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
					}
				}

				SceneComponent->UpdateComponentToWorld(EUpdateTransformFlags::None, ETeleportType::TeleportPhysics);
			}
		}
	}

	// Step five: Possess player character and turn other player into a bot
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (PlayerState != nullptr)
		{
			if (ALudeoPlayerController* LudeoPlayerController = Cast<ALudeoPlayerController>(PlayerState->GetOwner()))
			{
				if (APawn* CharacterPawn = LudeoPlayerController->GetCharacterPawn())
				{
					LudeoPlayerController->PlayerState->SetIsABot(LudeoPlayerController != World->GetFirstPlayerController());

					LudeoPlayerController->Possess(CharacterPawn);
				}
			}
		}
	}
}

void ALudeoGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALudeoGameState, ReplicatedLudeoRoomInformation);
}
