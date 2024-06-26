#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"

#include "LudeoUESDK/LudeoSession/LudeoSessionTypes.h"

#include "LudeoSessionBlueprintAsyncAction.generated.h"

UCLASS()
class LUDEOUESDK_API ULudeoDestroySessionAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FOnLudeoSessionDestroyedDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FOnLudeoSessionDestroyedDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Destroy Ludeo Session",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoDestroySessionAsyncNode* DestroyLudeoSession
	(
		UObject* WorldContextObject,
		const FDestroyLudeoSessionParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle);

	void OnResultReady(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle);

private:
	FDestroyLudeoSessionParameters Parameters;
};

UCLASS()
class LUDEOUESDK_API ULudeoSessionSubscribeNotificationAsyncNodeBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	virtual void Activate() override;

protected:
	virtual void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle);

protected:
	FLudeoSessionHandle SessionHandle;
};

UCLASS()
class LUDEOUESDK_API ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode : public ULudeoSessionSubscribeNotificationAsyncNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionOnLudeoSelectedDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionOnLudeoSelectedDynamicMulticastDelegate OnFailDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Ludeo Selected"))
	FLudeoSessionOnLudeoSelectedDynamicMulticastDelegate OnLudeoSelectedDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Subscribe to On Ludeo Selected Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode* SubscribeOnLudeoSelectedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle
	);

	virtual void Activate() override;

private:
	virtual void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle) override;
	void OnLudeoSelected(const FLudeoSessionHandle& InSessionHandle, const FString& LudeoID);
};

UCLASS()
class LUDEOUESDK_API ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode : public ULudeoSessionSubscribeNotificationAsyncNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnFailDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Pause Game Requested"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnPauseGameRequestedDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Subscribe to On Pause Game Requested Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode* SubscribeToOnPauseGameRequestedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle
	);

	virtual void Activate() override;

private:
	virtual void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle) override;
	void OnPauseGameRequested(const FLudeoSessionHandle& InSessionHandle);
};

UCLASS()
class LUDEOUESDK_API ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode : public ULudeoSessionSubscribeNotificationAsyncNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnFailDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Pause Game Requested"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnResumeGameRequestedDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Subscribe to On Resume Game Requested Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode* SubscribeToOnResumeGameRequestedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle
	);

	virtual void Activate() override;

private:
	virtual void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle) override;
	void OnResumeGameRequested(const FLudeoSessionHandle& InSessionHandle);
};

UCLASS()
class LUDEOUESDK_API ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode : public ULudeoSessionSubscribeNotificationAsyncNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnFailDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Pause Game Requested"))
	FLudeoSessionNotificationDynamicMulticastDelegate OnGameBackToMainMenuRequestedDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Subscribe to On Game back to Main Menu Requested Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode* SubscribeToOnGameBackToMainMenuRequestedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle
	);

	virtual void Activate() override;

private:
	virtual void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle) override;
	void OnGameBackToMainMenuRequested(const FLudeoSessionHandle& InSessionHandle);
};

UCLASS()
class LUDEOUESDK_API ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode : public ULudeoSessionSubscribeNotificationAsyncNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionOnRoomReadyDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionOnRoomReadyDynamicMulticastDelegate OnFailDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Pause Game Requested"))
	FLudeoSessionOnRoomReadyDynamicMulticastDelegate OnRoomReadyDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Subscribe to On Room Ready Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode* SubscribeToOnRoomReadyNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle
	);

	virtual void Activate() override;

private:
	virtual void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle) override;
	void OnRoomReady(const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle);
};

UCLASS()
class LUDEOUESDK_API ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode : public ULudeoSessionSubscribeNotificationAsyncNodeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionOnPlayerConsentUpdatedDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionOnPlayerConsentUpdatedDynamicMulticastDelegate OnFailDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Pause Game Requested"))
	FLudeoSessionOnPlayerConsentUpdatedDynamicMulticastDelegate OnPlayerConsentUpdatedDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Subscribe to On Player Consent Updated Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode* SubscribeToOnPlayerConsentUpdatedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle
	);

	virtual void Activate() override;

private:
	virtual void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle) override;
	void OnPlayerConsentUpdated(const FLudeoSessionHandle& InSessionHandle, const FLudeoSessionPlayerConsentData& InPlayerConsentData);
};

UCLASS()
class ULudeoSessionActivateSessionAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionOnActivatedDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionOnActivatedDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Activate Ludeo Session",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionActivateSessionAsyncNode* ActivateSession
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionActivateSessionParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnActivated(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const bool bIsLudeoSelected);

	void OnResultReady
	(
		const FLudeoResult& Result,
		const FLudeoSessionHandle& InSessionHandle,
		const bool bIsLudeoSelected
	);

private:
	FLudeoSessionHandle SessionHandle;
	FLudeoSessionActivateSessionParameters Parameters;
};

UCLASS()
class ULudeoSessionGetLudeoAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionOnGetLudeoDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionOnGetLudeoDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Get Ludeo",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionGetLudeoAsyncNode* GetLudeo
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionGetLudeoParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnGetLudeo(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoHandle& LudeoHandle);

	void OnResultReady
	(
		const FLudeoResult& Result,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoHandle& LudeoHandle
	);

private:
	FLudeoSessionHandle SessionHandle;
	FLudeoSessionGetLudeoParameters Parameters;
};

UCLASS()
class ULudeoSessionOpenRoomAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionOnOpenRoomDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionOnOpenRoomDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Open Room",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionOpenRoomAsyncNode* OpenRoom
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionOpenRoomParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnOpenRoom(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle);

	void OnResultReady(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle);

private:
	FLudeoSessionHandle SessionHandle;
	FLudeoSessionOpenRoomParameters Parameters;
};

UCLASS()
class ULudeoSessionCloseRoomAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoSessionOnCloseRoomDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoSessionOnCloseRoomDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session Close Room",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionCloseRoomAsyncNode* CloseRoom
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionCloseRoomParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnCloseRoom(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle);

	void OnResultReady(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle);

private:
	FLudeoSessionHandle SessionHandle;
	FLudeoSessionCloseRoomParameters Parameters;
};
