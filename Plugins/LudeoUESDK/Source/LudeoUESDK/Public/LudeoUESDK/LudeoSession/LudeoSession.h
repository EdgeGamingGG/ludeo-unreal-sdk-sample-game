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

	void GetLudeo
	(
		const FString& LudeoID,
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
	static void StaticOnGetLudeo(const LudeoSessionGetLudeoCallbackParams* pGetLudeoCallbackParams);
	void OnGetLudeo(const LudeoSessionGetLudeoCallbackParams& GetLudeoCallbackParams);

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

	void OnSessionActivated(const LudeoSessionActivateCallbackParams& SessionActivateCallbackParams) const;
	static void StaticOnSessionActivated(const LudeoSessionActivateCallbackParams* pSessionActivateCallbackParams);

	void OnRoomOpen(const LudeoSessionOpenRoomCallbackParams& OpenRoomCallbackParams);
	static void StaticOnRoomOpen(const LudeoSessionOpenRoomCallbackParams* pOpenRoomCallbackParams);

	void OnRoomClose(const LudeoRoomCloseCallbackParams& CloseRoomCallbackParams);
	static void StaticOnRoomClose(const LudeoRoomCloseCallbackParams* pCloseRoomCallbackParams);

	void OnMarkHighlight(const LudeoSessionMarkHighlightCallbackParams& MarkHighlightCallbackParams) const;
	static void StaticOnMarkHighlight(const LudeoSessionMarkHighlightCallbackParams* pMarkHighlightCallbackParams);

private:
	FOnLudeoSessionDestroyedMulticastDelegate				OnDestroySessionDelegate;
	FLudeoSessionOnLudeoSelectedMulticastDelegate			OnLudeoSelectedDelegate;
	FLudeoSessionOnPauseGameRequestedMulticastDelegate		OnPauseGameRequestedDelegate;
	FLudeoSessionOnResumeGameRequestedMulticastDelegate		OnResumeGameRequestedDelegate;
	FLudeoSessionOnGameBackToMenuRequestedMulticastDelegate OnGameBackToMenuRequestedDelegate;
	FLudeoSessionOnRoomReadyMulticastDelegate				OnRoomReadyDelegate;

private:
	FLudeoSessionHandle SessionHandle;

private:
	TArray<FLudeoRoom> RoomCollection;
	TArray<FLudeo> LudeoCollection;
	TArray<LudeoNotificationId, TInlineAllocator<5>> NotificationIDCollection;
};
