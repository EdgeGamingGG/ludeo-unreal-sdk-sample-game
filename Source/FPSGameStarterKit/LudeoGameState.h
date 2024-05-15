#pragma once

#include "GameFramework/GameState.h"

#include "LudeoUESDK/LudeoSession/LudeoSession.h"
#include "LudeoUESDK/LudeoObject/LudeoObjectStateManager.h"

#include "LudeoGameState.generated.h"

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
class ALudeoGameState : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FLudeoSaveGameSpecification SaveGameSpecification;

	UPROPERTY(Transient, ReplicatedUsing=OnRep_LudeoRoomInformation)
	FReplicatedLudeoRoomInformation ReplicatedLudeoRoomInformation;

public:
	ALudeoGameState();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void HandleMatchIsWaitingToStart() override;

	void LoadLudeo(const FLudeo& Ludeo);

	UFUNCTION(BlueprintCallable)
	bool ReportPlayerAction(const APlayerState* PlayerState, const ELudeoPlayerAction PlayerAction) const;

	FORCEINLINE bool IsLudeoGame() const
	{
		return !ReplicatedLudeoRoomInformation.RoomInformation.LudeoID.IsEmpty();
	}

	bool IsSessionReady() const;
	
private:
	UFUNCTION()
	void OnRep_LudeoRoomInformation();

	void TickSaveObjectState();

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
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo",
			DisplayName = "Ludeo Report Local Player Action",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static bool ReportLocalPlayerAction(const UObject* WorldContextObject, const ELudeoPlayerAction PlayerAction);

	const APlayerState* GetLocalPlayerState() const;

	void OnSessionReady(const bool bIsSuccessful);

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

private:
	FLudeoWritableObject::WritableObjectMapType ObjectMap;

private:
	FLudeoSessionHandle LudeoSessionHandle;
	FLudeoRoomHandle LudeoRoomHandle;
	FLudeoPlayerHandle LudeoPlayerHandle;
};
