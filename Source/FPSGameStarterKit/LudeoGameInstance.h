#pragma once

#include "Engine/GameInstance.h"

#include "LudeoManager/LudeoManager.h"

#include "LudeoGameInstance.generated.h"

UCLASS(config=Game)
class ULudeoGameInstance : public UGameInstance
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString PendingLudeoIDToLoad;

public:
	ULudeoGameInstance();

	virtual void Init() override;

	virtual void Shutdown() override;

	const FString& GetPendingLudeoIDToLoad() const
	{
		return PendingLudeoIDToLoad;
	}

	void SetPendingLudeoIDToLoad(const FString& LudeoID)
	{
		PendingLudeoIDToLoad = LudeoID;
	}

	const FLudeoSessionHandle& GetActiveSessionHandle() const
	{
		return LudeoSessionHandle;
	}

	bool SetupLudeoSession
	(
		const FLudeoSessionOnActivatedDelegate& OnSessionActivatedDelegate,
		const FLudeoSessionOnLudeoSelectedMulticastDelegate::FDelegate& OnLudeoSelectedDelegate
	);

	void DestoryLudeoSession(const FOnLudeoSessionDestroyedDelegate& OnSessionDestroyedDelegate);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void Tick(float DeltaSeconds);

	bool NativeTick(float DeltaSeconds);

private:
	void OnLudeoSessionActivated
	(
		const FLudeoResult& Result,
		const FLudeoSessionHandle& SessionHandle,
		const bool bIsLudeoSelected
	);

	void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle);

private:
	TWeakPtr<FLudeoManager> WeakLudeoManager;

	/** Delegate handle for callbacks to Tick */
	FDelegateHandle TickDelegateHandle;

private:
	FLudeoSessionHandle LudeoSessionHandle;
};

