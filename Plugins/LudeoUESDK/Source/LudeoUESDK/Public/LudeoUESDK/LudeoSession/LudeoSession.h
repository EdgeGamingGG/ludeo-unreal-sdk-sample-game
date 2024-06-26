#pragma once

#include "LudeoUESDK/LudeoSession/LudeoSessionTypes.h"
#include "LudeoUESDK/Ludeo/Ludeo.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

class LUDEOUESDK_API FLudeoSession
{
	friend class FLudeoRoom;
	friend class ULudeoSessionSubscribeNotificationAsyncNodeBase;

public:
	FLudeoSession(const FLudeoSessionHandle& InSessionHandle);
	~FLudeoSession();
	
	FORCEINLINE operator const FLudeoSessionHandle&() const
	{
		return SessionHandle;
	}

	static FLudeoSession* GetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle);

	FLudeo* GetLudeoByLudeoHandle(const FLudeoHandle& LudeoHandle) const;

	FLudeoRoom* GetRoomByRoomHandle(const FLudeoRoomHandle& RoomHandle) const;
	FLudeoRoom* GetRoomByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle) const;

	FORCEINLINE FLudeoSessionOnLudeoSelectedMulticastDelegate& GetOnLudeoSelectedDelegate()
	{
		return OnLudeoSelectedDelegate;
	}

	FORCEINLINE FLudeoSessionOnPauseGameRequestedMulticastDelegate& GetOnPauseGameRequestedDelegate()
	{
		return OnPauseGameRequestedDelegate;
	}

	FORCEINLINE FLudeoSessionOnResumeGameRequestedMulticastDelegate& GetOnResumeGameRequestedDelegate()
	{
		return OnResumeGameRequestedDelegate;
	}

	FORCEINLINE FLudeoSessionOnGameBackToMenuRequestedMulticastDelegate& GetOnGameBackToMenuRequestedDelegate()
	{
		return OnGameBackToMenuRequestedDelegate;
	}

	FORCEINLINE FLudeoSessionOnRoomReadyMulticastDelegate& GetOnRoomReadyDelegate()
	{
		return OnRoomReadyDelegate;
	}

	FORCEINLINE FLudeoSessionOnPlayerConsentUpdatedMulticastDelegate& GetOnPlayerConsentUpdatedDelegate()
	{
		return OnPlayerConsentUpdatedDelegate;
	}

	void GetLudeo
	(
		const FLudeoSessionGetLudeoParameters& GetLudeoParameters,
		const FLudeoSessionOnGetLudeoDelegate& OnGetLudeoDelegate = {}
	) const;

	bool ReleaseLudeo(const FLudeoHandle& LudeoHandle);

	void Activate
	(
		const FLudeoSessionActivateSessionParameters& ActivateSessionParameters,
		const FLudeoSessionOnActivatedDelegate& OnActivatedDelegate = {}
	);

	void OpenRoom
	(
		const FLudeoSessionOpenRoomParameters& OpenRoomParameters,
		const FLudeoSessionOnOpenRoomDelegate& OnOpenRoomDelegate = {}
	) const;

	void CloseRoom
	(
		const FLudeoSessionCloseRoomParameters& CloseRoomParameters,
		const FLudeoSessionOnCloseRoomDelegate& OnCloseRoomDelegate = {}
	) const;

	FLudeoResult OpenGallery(const FLudeoSessionOpenGalleryParameters& OpenGalleryParameters = {}) const;

	FORCEINLINE bool IsValid() const
	{
		return (this == FLudeoSession::GetSessionBySessionHandle(SessionHandle));
	}

private:
	FORCEINLINE FOnLudeoSessionDestroyedMulticastDelegate& GetOnDestroySessionDelegate()
	{
		return OnDestroySessionDelegate;
	}

private:
	void SubscribeNotification();

private:
	static void StaticOnLudeoSelected(const LudeoSessionLudeoSelectedCallbackParams* pLudeoSelectedCallbackParams);
	void OnLudeoSelected(const LudeoSessionLudeoSelectedCallbackParams& LudeoSelectedCallbackParams) const;

	static void StaticOnPauseGameRequested(const LudeoSessionPauseGameRequestCallbackParams* pPauseGameRequestCallbackParams);
	void OnPauseGameRequested(const ::LudeoSessionPauseGameRequestCallbackParams& PauseGameRequestCallbackParams) const;

	static void StaticOnResumeGameRequested(const LudeoSessionResumeGameRequestCallbackParams* pPauseGameRequestCallbackParams);
	void OnResumeGameRequested(const LudeoSessionResumeGameRequestCallbackParams& PauseGameRequestCallbackParams) const;

	static void StaticOnGameBackToMenuRequested(const LudeoSessionBackToMenuRequestCallbackParams* pGameBackToMenuRequestCallbackParams);
	void OnGameBackToMenuRequested(const LudeoSessionBackToMenuRequestCallbackParams& GameBackToMenuRequestCallbackParams) const;

	static void StaticOnRoomReady(const LudeoSessionRoomReadyCallbackParams* pRoomReadyCallbackParams);
	void OnRoomReady(const LudeoSessionRoomReadyCallbackParams& RoomReadyCallbackParams) const;

	static void StaticOnPlayerConsentUpdated(const LudeoSessionConsentUpdatedCallbackParams* pConsentUpdatedCallbackParams);
	void OnPlayerConsentUpdated(const LudeoSessionConsentUpdatedCallbackParams& ConsentUpdatedCallbackParams) const;

private:
	FOnLudeoSessionDestroyedMulticastDelegate				OnDestroySessionDelegate;
	FLudeoSessionOnLudeoSelectedMulticastDelegate			OnLudeoSelectedDelegate;
	FLudeoSessionOnPauseGameRequestedMulticastDelegate		OnPauseGameRequestedDelegate;
	FLudeoSessionOnResumeGameRequestedMulticastDelegate		OnResumeGameRequestedDelegate;
	FLudeoSessionOnGameBackToMenuRequestedMulticastDelegate OnGameBackToMenuRequestedDelegate;
	FLudeoSessionOnRoomReadyMulticastDelegate				OnRoomReadyDelegate;
	FLudeoSessionOnPlayerConsentUpdatedMulticastDelegate	OnPlayerConsentUpdatedDelegate;

private:
	FLudeoSessionHandle SessionHandle;

private:
	TArray<FLudeoRoom> RoomCollection;
	TArray<FLudeo> LudeoCollection;
	TArray<LudeoNotificationId, TInlineAllocator<6>> NotificationIDCollection;
};
