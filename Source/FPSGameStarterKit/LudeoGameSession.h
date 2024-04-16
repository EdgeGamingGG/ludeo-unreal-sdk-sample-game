#pragma once

#include "GameFramework/GameSession.h"

#include "LudeoSession/LudeoSession.h"
#include "LudeoObject/LudeoObjectStateManager.h"

#include "LudeoGameSession.generated.h"

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

UCLASS(Blueprintable)
class ALudeoGameSession : public AGameSession
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FLudeoSaveGameSpecification SaveGameSpecification;

public:
	ALudeoGameSession();

	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure)
	FLudeoSessionHandle GetActiveLudeoSessionHandle() const;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Game Session",
			DisplayName = "Ludeo Report Player Action",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static bool ReportPlayerAction(const UObject* WorldContextObject, const int32 PlayerIndex, const ELudeoPlayerAction PlayerAction);

	bool IsSessionReady() const;

private:
	virtual void HandleMatchIsWaitingToStart() override;
	virtual void HandleMatchHasStarted() override;
	virtual void PostLogin(APlayerController* NewPlayer);

private:
	bool InternalReportPlayerAction(const int32 PlayerIndex, const ELudeoPlayerAction PlayerAction);

	void OnSessionReady(const bool bIsSuccessful);

	void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle);
	void OnLudeoPlayerBeginGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& PlayerHandle);
	void OnLudeoPlayerAdded(const FLudeoResult& Result, const FLudeoRoomHandle& RoomHandle, const FLudeoPlayerHandle& PlayerHandle);
	void OnLudeoRoomOpened(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle);
	void OnLudeoSessionActivated(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const bool bIsLudeoSelected);
	void OnGetLudeo(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoHandle& InLudeoHandle);

	void OnLudeoSelected(const FLudeoSessionHandle& SessionHandle, const FString& LudeoID);
	void OnPauseGameRequested(const FLudeoSessionHandle& SessionHandle);
	void OnResumeGameRequested(const FLudeoSessionHandle& SessionHandle);
	void OnGameBackToMainMenuRequested(const FLudeoSessionHandle& SessionHandle);
	void OnLudeoRoomReady(const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle);

private:
	void OpenRoom(const FString& LudeoID);
	void CloseRoom();
	void AddPlayer(FLudeoRoom& LudeoRoom, const int32 PlayerIndex);
	void AllPlayerBeginGameplay();

	const AGameStateBase* GetGameState() const;

private:
	bool bIsLudeoGameSessionReady;

	FLudeoObjectStateManager LudeoObjectStateManager;

private:
	TWeakPtr<class FLudeoManager> WeakLudeoManager;

	FLudeoSessionHandle LudeoSessionHandle;
	FLudeoHandle LudeoHandle;
	FLudeoRoomHandle LudeoRoomHandle;
	
	FLudeoObjectStateManager::PlayerMapType LudeoPlayerMap;
	int32 PendingNumberOfPlayerAdded;

	bool bIsReloadingLudeo;
};
