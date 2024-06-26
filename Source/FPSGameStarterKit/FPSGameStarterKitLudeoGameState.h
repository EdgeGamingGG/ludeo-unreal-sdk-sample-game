#pragma once

#include "GameFramework/GameState.h"

#include "LudeoUESDK/LudeoSession/LudeoSession.h"
#include "LudeoUESDK/LudeoObject/LudeoObjectStateManager.h"

#include "FPSGameStarterKitLudeoGameState.generated.h"

UENUM(BlueprintType)
enum class ELudeoPlayerAction : uint8
{
	PlayerCollectMoney		UMETA(DisplayName = "Player Collect Money"),
	PlayerCollectItem		UMETA(DisplayName = "Player Collect Item"),

	PlayerShoot				UMETA(DisplayName = "Player Shoot"),
	PlayerVehicleShoot		UMETA(DisplayName = "Player Vehicle Shoot"),
	PlayerDamageTaken		UMETA(DisplayName = "Player Damage Taken"),
	PlayerKill				UMETA(DisplayName = "Player Kill"),
	PlayerDie				UMETA(DisplayName = "Player Die"),

	PlayerEnterVehicle		UMETA(DisplayName = "Player Enter Vehicle"),
	PlayerLeaveVehicle		UMETA(DisplayName = "Player Leave Vehicle"),

	OpenPauseMenu			UMETA(DisplayName = "Open Pause Menu"),
	ClosePauseMenu			UMETA(DisplayName = "Close Pause Menu"),
};
Expose_TNameOf(ELudeoPlayerAction);

USTRUCT()
struct FReplicatedLudeoRoomInformation
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	FLudeoRoomInformation RoomInformation;

	UPROPERTY(Transient)
	FLudeoResult OpenRoomResult;

	UPROPERTY(Transient)
	bool bIsRoomResultReady = false;
};

UCLASS(Blueprintable)
class AFPSGameStarterKitLudeoGameState : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FLudeoSaveGameSpecification SaveGameSpecification;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_LudeoRoomInformation)
	FReplicatedLudeoRoomInformation ReplicatedLudeoRoomInformation;

public:
	AFPSGameStarterKitLudeoGameState();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void HandleMatchIsWaitingToStart() override;

	void LoadLudeo(const FLudeo& Ludeo);

	bool IsSessionReady() const;

public:
	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			DisplayName = "Ludeo Report Player Action"
		)
	)
	static bool ReportPlayerAction(const APlayerState* PlayerState, const ELudeoPlayerAction PlayerAction);

	UFUNCTION
	(
		BlueprintPure,
		meta =
		(
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static bool IsLudeoGame(const UObject* WorldContextObject);

	UFUNCTION
	(
		BlueprintPure,
		meta =
		(
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static const APlayerState* GetLocalPlayerState(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	static const APlayerState* GetObjectAssociatedPlayerState(const UObject* Object);

private:
	UFUNCTION()
	void OnRep_LudeoRoomInformation();

	void OnActorSpawned(AActor* ActorSpawned, const bool bShouldCheckSaveGameActor);

	UFUNCTION()
	void OnActorDestroyed(AActor* DestroyedActor);

	void OnPlayerLeft(class AGameModeBase*, class AController* Controller);

	void TickSaveObjectState();

	void OnSessionReady(const FLudeoResult& Result);

private:
	void OnLudeoSessionActivated(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const bool bIsLudeoSelected);
	void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle);
	void OnLudeoPlayerBeginGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& PlayerHandle);
	void OnLudeoPlayerAdded(const FLudeoResult& Result, const FLudeoRoomHandle& RoomHandle, const FLudeoPlayerHandle& PlayerHandle);
	void OnLudeoRoomOpened(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle);

	void OnLudeoSelected(const FLudeo& Ludeo);
	void OnLudeoRoomReady(const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle);

private:
	void ConditionalOpenRoom();

	void SetupSession();
	void OpenRoom(const FString& RoomID, const FString& LudeoID);
	void CloseRoom();
	void AddPlayer(const APlayerState& PlayerState, FLudeoRoom& LudeoRoom);
	void RemovePlayer(const APlayerState& PlayerState, FLudeoRoom& LudeoRoom);
	void BeginGamePlay();
	void EndGamePlay();

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	enum class ELudeoGameSessionInitializationState : uint8
	{
		NotInitialized,
		SessionSetupOnTheFly,
		WaitingForRoomSetup,
		RoomSetupOnTheFly,
		PlayerSetupOnTheFly,
		Succeeded,
		Failed
	} LudeoGameSessionInitializationState;
	FLudeoResult LudeoGameSessionInitializationResult;

private:
	FDelegateHandle ActorSpawnedDelegateHandle;

	TOptional<TArray<AActor*>> SaveGameActorCollection;
	FLudeoWritableObject::WritableObjectMapType ObjectMap;

private:
	FLudeoSessionHandle LudeoSessionHandle;
	FLudeoRoomHandle LudeoRoomHandle;
	FLudeoPlayerHandle LudeoPlayerHandle;
};
