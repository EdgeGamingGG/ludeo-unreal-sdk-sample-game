#pragma once

#include "LudeoSessionTypes.h"
#include "Ludeo/Ludeo.h"
#include "LudeoRoom/LudeoRoom.h"

struct LUDEOUESDK_API FLudeoSession
{
	friend struct FLudeoRoom;
	friend struct FLudeoDataReader;
	friend class ULudeoSessionSubscribeNotificationAsyncNodeBase;

public:
	FLudeoSession(const FLudeoSessionHandle& InSessionHandle);

	~FLudeoSession();
	
	FORCEINLINE operator FLudeoSessionHandle() const
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

	bool ReleaseLudeo(const FLudeoHandle& LudeoHandle);

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
	TArray<FLudeoRoom> RoomCollection;

	TArray<FLudeo> LudeoCollection;

	FLudeoSessionHandle SessionHandle;
	TArray<LudeoNotificationId, TInlineAllocator<5>> NotificationIDCollection;
};
