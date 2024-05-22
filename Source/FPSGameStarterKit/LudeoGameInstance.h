#pragma once

#include "Engine/GameInstance.h"

#include "LudeoUESDK/LudeoManager/LudeoManager.h"

#include "LudeoGameInstance.generated.h"

UCLASS(config=Game)
class ULudeoGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowedClasses = "World"))
	FSoftObjectPath MainMenuPath;

public:
	ULudeoGameInstance();

	virtual void Init() override;

	virtual void Shutdown() override;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static bool OpenLudeoGallery(const FLudeoSessionHandle& LudeoSessionHandle);

	UFUNCTION(exec, BlueprintCallable)
	static bool ExecuteLudeoCommand(const FString& Key, const FString& Value);

	UFUNCTION
	(
		BlueprintPure,
		meta =
		(
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static FLudeoSessionHandle GetActiveLudeoSessionHandle(const UObject* WorldContextObject);

	bool HasPendingSessionActivation() const
	{
		return (LudeoSessionHandle.IsSet() && LudeoSessionHandle.GetValue() == nullptr);
	}

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
	void LoadLudeo(const FString& LudeoID);

	const FLudeoHandle& GetPendingLudeoHandle() const
	{
		return PendingLudeoHandle;
	}

	void MarkLudeoAsPending(const FLudeo& Ludeo);

	bool ReleasePendingLudeo();

	bool SetupLudeoSession(const FLudeoSessionOnActivatedDelegate& OnSessionActivatedDelegate = FLudeoSessionOnActivatedDelegate());

	void DestoryLudeoSession(const FOnLudeoSessionDestroyedDelegate& OnSessionDestroyedDelegate = FOnLudeoSessionDestroyedDelegate());

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void Tick(float DeltaSeconds);

	bool NativeTick(float DeltaSeconds);

private:
	FString GetLudeoMapName(const FLudeo& Ludeo) const;
	void OnGetLudeo(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoHandle& LudeoHandle);
	void OnLudeoSelected(const FLudeoSessionHandle& SessionHandle, const FString& LudeoID);
	void OnPauseGameRequested(const FLudeoSessionHandle& SessionHandle);
	void OnResumeGameRequested(const FLudeoSessionHandle& SessionHandle);
	void OnGameBackToMainMenuRequested(const FLudeoSessionHandle& SessionHandle);

	void OnLudeoSessionActivated
	(
		const FLudeoResult& Result,
		const FLudeoSessionHandle& SessionHandle,
		const bool bIsLudeoSelected,
		const FLudeoSessionOnActivatedDelegate OnSessionActivatedDelegate
	);

private:
	int32 GameInstanceRegistrationID;

private:
	TWeakPtr<FLudeoManager> WeakLudeoManager;

	/** Delegate handle for callbacks to Tick */
	FDelegateHandle TickDelegateHandle;

private:
	TOptional<FLudeoSessionHandle> LudeoSessionHandle;

	FLudeoHandle PendingLudeoHandle;
};

