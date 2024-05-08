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

private:
	void OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle);

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
	static ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode* LudeoSessionSubscribeToOnLudeoSelectedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionSubscribeToOnLudeoSelectedNotificationParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnLudeoSelected(const FLudeoSessionHandle& InSessionHandle, const FString& LudeoID);

private:
	FLudeoSessionSubscribeToOnLudeoSelectedNotificationParameters Parameters;
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
	static ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode* LudeoSessionSubscribeToOnPauseGameRequestedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionSubscribeToOnPauseGameRequestedNotificationParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnPauseGameRequested(const FLudeoSessionHandle& InSessionHandle);

private:
	FLudeoSessionSubscribeToOnPauseGameRequestedNotificationParameters Parameters;
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
	static ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode* LudeoSessionSubscribeToOnResumeGameRequestedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionSubscribeToOnResumeGameRequestedNotificationParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnResumeGameRequested(const FLudeoSessionHandle& InSessionHandle);

private:
	FLudeoSessionSubscribeToOnResumeGameRequestedNotificationParameters Parameters;
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
			DisplayName = "Ludeo Session Subscribe to On Resume Game Requested Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode* LudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnGameBackToMainMenuRequested(const FLudeoSessionHandle& InSessionHandle);

private:
	FLudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationParameters Parameters;
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
			DisplayName = "Ludeo Session Subscribe to On Resume Game Requested Notification",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode* LudeoSessionSubscribeToOnRoomReadyNotification
	(
		UObject* WorldContextObject,
		const FLudeoSessionHandle& InSessionHandle,
		const FLudeoSessionSubscribeToOnRoomReadyNotificationParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnRoomReady(const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle);

private:
	FLudeoSessionSubscribeToOnRoomReadyNotificationParameters Parameters;
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
	static ULudeoSessionActivateSessionAsyncNode* ActivateLudeoSession
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
	static ULudeoSessionOpenRoomAsyncNode* LudeoSessionOpenRoom
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
	static ULudeoSessionCloseRoomAsyncNode* LudeoSessionCloseRoom
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
