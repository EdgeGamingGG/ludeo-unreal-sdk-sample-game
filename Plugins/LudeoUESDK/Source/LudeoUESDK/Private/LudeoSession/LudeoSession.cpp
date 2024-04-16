#include "LudeoSession/LudeoSession.h"

#include <Ludeo/Session.h>
#include <Ludeo/DataReader.h>
#include <Ludeo/Room.h>
#include <Ludeo/TemporaryAPI.h>
#include <Ludeo/Utils.h>

#include "LudeoSession/LudeoSessionManager.h"
#include "LudeoRoom/LudeoRoom.h"

struct FLudeoSessionPendingData
{
	FLudeoSessionHandle SessionHandle;
};

struct FLudeoSessionPendingGetLudeoData : public FLudeoSessionPendingData
{
	FString LudeoID;
	FLudeoSessionOnGetLudeoDelegate OnGetLudeoDelegate;
};

struct FLudeoSessionPendingActivateSessionData : public FLudeoSessionPendingData
{
	FLudeoSessionOnActivatedDelegate OnActivatedDelegate;
};

struct FLudeoSessionPendingOpenRoomData : public FLudeoSessionPendingData
{
	FLudeoSessionOnOpenRoomDelegate OnOpenRoomDelegate;
};

struct FLudeoSessionPendingCloseRoomData : public FLudeoSessionPendingData
{
	FLudeoRoomHandle RoomHandle;
	FLudeoSessionOnCloseRoomDelegate OnCloseRoomDelegate;
};

FLudeoSession::FLudeoSession(const FLudeoSessionHandle& InSessionHandle) :
	SessionHandle(InSessionHandle)
{

}

FLudeoSession::~FLudeoSession()
{
	LudeoCollection.Empty();

	for (const LudeoNotificationId& NotificationID : NotificationIDCollection)
	{
		ludeo_RemoveNotification(NotificationID);
	}
	NotificationIDCollection.Empty();
	
	while (RoomCollection.Num() > 0)
	{
		const LudeoRoomCloseParams RoomCloseParams = Ludeo::create<LudeoRoomCloseParams>();

		const int32 Index = RoomCollection.Num() - 1;

		const FLudeoRoomHandle& RoomHandle = RoomCollection[Index];

		RoomCollection.RemoveAt(Index, 1, false);

		ludeo_Room_Close
		(
			RoomHandle,
			&RoomCloseParams,
			nullptr,
			[](const LudeoRoomCloseCallbackParams*)
			{
				// I don't care since this is being shutdown
			}
		);
	}

	OnDestroySessionDelegate.Broadcast(LudeoResult::Success, SessionHandle);
}

FLudeo* FLudeoSession::GetLudeoByLudeoHandle(const FLudeoHandle& LudeoHandle) const
{
	const int32 Index = LudeoCollection.IndexOfByPredicate([&](const FLudeo& Ludeo)
	{
		return (static_cast<FLudeoHandle>(Ludeo) == LudeoHandle);
	});

	if (LudeoCollection.IsValidIndex(Index))
	{
		return const_cast<FLudeo*>(&LudeoCollection[Index]);
	}

	return nullptr;
}

void FLudeoSession::GetLudeo(const FString& LudeoID, const FLudeoSessionOnGetLudeoDelegate& OnGetLudeoDelegate) const
{
	const FTCHARToUTF8 LudeoIDStringConverter
	(
		LudeoID.GetCharArray().GetData(),
		LudeoID.GetCharArray().Num()
	);

	LudeoSessionGetLudeoParams GetLudeoParameters = Ludeo::create<LudeoSessionGetLudeoParams>();
	GetLudeoParameters.ludeoId = LudeoIDStringConverter.Get();

	FLudeoSessionPendingGetLudeoData* PendingGetLudeoData = new FLudeoSessionPendingGetLudeoData;
	PendingGetLudeoData->LudeoID = LudeoID;
	PendingGetLudeoData->OnGetLudeoDelegate = OnGetLudeoDelegate;
	PendingGetLudeoData->SessionHandle = SessionHandle;

	ludeo_Session_GetLudeo
	(
		SessionHandle,
		&GetLudeoParameters,
		PendingGetLudeoData,
		&FLudeoSession::StaticOnGetLudeo
	);
}

void FLudeoSession::Activate
(
	const FLudeoSessionActivateSessionParameters& ActivateSessionParameters,
	const FLudeoSessionOnActivatedDelegate& OnActivatedDelegate
)
{
	SubscribeNotification();
	check(NotificationIDCollection.Num() == NotificationIDCollection.Max());

	const FTCHARToUTF8 APIKeyStringConverter
	(
		ActivateSessionParameters.ApiKey.GetCharArray().GetData(),
		ActivateSessionParameters.ApiKey.GetCharArray().Num()
	);

	const FTCHARToUTF8 AppTokenStringConverter
	(
		ActivateSessionParameters.AppToken.GetCharArray().GetData(),
		ActivateSessionParameters.AppToken.GetCharArray().Num()
	);

	LudeoSessionActivateParams InternalSessionActivateParams = Ludeo::create<LudeoSessionActivateParams>();
	InternalSessionActivateParams.platformUrl = "https://services.ludeo.com";

#if !UE_BUILD_SHIPPING
	FString PlatformURL;
	
	if(FParse::Value(FCommandLine::Get(), TEXT("PlatformURL="), PlatformURL))
	{
		const FTCHARToUTF8 PlatformURLConverter(PlatformURL.GetCharArray().GetData(), PlatformURL.GetCharArray().Num());

 		InternalSessionActivateParams.platformUrl = PlatformURLConverter.Get();
	}
#endif

 	InternalSessionActivateParams.apiKey = APIKeyStringConverter.Get();
 	InternalSessionActivateParams.appToken = AppTokenStringConverter.Get();

	InternalSessionActivateParams.windowHandle = ActivateSessionParameters.GameWindowHandle;
	InternalSessionActivateParams.reset = static_cast<LudeoBool>(ActivateSessionParameters.bResetAttributeAndAction);

	FLudeoSessionPendingActivateSessionData* PendingActivateSessionData = new FLudeoSessionPendingActivateSessionData;
	PendingActivateSessionData->OnActivatedDelegate = OnActivatedDelegate;
	PendingActivateSessionData->SessionHandle = SessionHandle;

	ludeo_Session_Activate(SessionHandle, &InternalSessionActivateParams, PendingActivateSessionData, &FLudeoSession::StaticOnSessionActivated);
}

void FLudeoSession::OpenRoom
(
	const FLudeoSessionOpenRoomParameters& OpenRoomParameters,
	const FLudeoSessionOnOpenRoomDelegate& OnOpenRoomDelegate
) const
{
	const FTCHARToUTF8 RoomIDStringConverter
	(
		OpenRoomParameters.RoomID.GetCharArray().GetData(),
		OpenRoomParameters.RoomID.GetCharArray().Num()
	);

	const FTCHARToUTF8 LudeoIDStringConverter
	(
		OpenRoomParameters.LudeoID.GetCharArray().GetData(),
		OpenRoomParameters.LudeoID.GetCharArray().Num()
	);

	LudeoSessionOpenRoomParams IntenralOpenRoomParams = Ludeo::create<LudeoSessionOpenRoomParams>();
	IntenralOpenRoomParams.roomId = RoomIDStringConverter.Get();
	IntenralOpenRoomParams.ludeoId = LudeoIDStringConverter.Get();

	FLudeoSessionPendingOpenRoomData* PendingOpenRoomData = new FLudeoSessionPendingOpenRoomData;
	PendingOpenRoomData->OnOpenRoomDelegate = OnOpenRoomDelegate;
	PendingOpenRoomData->SessionHandle = SessionHandle;

	ludeo_Session_OpenRoom(SessionHandle, &IntenralOpenRoomParams, PendingOpenRoomData, &FLudeoSession::StaticOnRoomOpen);
}

void FLudeoSession::CloseRoom
(
	const FLudeoSessionCloseRoomParameters& CloseRoomParameters,
	const FLudeoSessionOnCloseRoomDelegate& OnCloseRoomDelegate
) const
{
	LudeoRoomCloseParams IntenralCloseRoomParams = Ludeo::create<LudeoRoomCloseParams>();

	FLudeoSessionPendingCloseRoomData* PendingCloseRoomData = new FLudeoSessionPendingCloseRoomData;
	PendingCloseRoomData->RoomHandle = CloseRoomParameters.RoomHandle;
	PendingCloseRoomData->OnCloseRoomDelegate = OnCloseRoomDelegate;
	PendingCloseRoomData->SessionHandle = SessionHandle;

	ludeo_Room_Close(CloseRoomParameters.RoomHandle, &IntenralCloseRoomParams, PendingCloseRoomData, &FLudeoSession::StaticOnRoomClose);
}

void FLudeoSession::SubscribeNotification()
{
	if (NotificationIDCollection.Num() == 0)
	{
		// On Ludeo Selected
		{
			const LudeoSessionAddNotifyLudeoSelectedParams AddNotifyLudeoSelectedParams = Ludeo::create<LudeoSessionAddNotifyLudeoSelectedParams>();

			const LudeoNotificationId NotificationID = ludeo_Session_AddNotifyLudeoSelected
			(
				SessionHandle,
				&AddNotifyLudeoSelectedParams,
				static_cast<LudeoHSession>(SessionHandle),
				&FLudeoSession::StaticOnLudeoSelected
			);

			check(NotificationID != LUDEO_INVALID_NOTIFICATIONID);

			NotificationIDCollection.Add(NotificationID);
		}

		// On Pause Game Requested
		{
			const LudeoSessionAddNotifyPauseGameRequestParams AddNotifyPauseGameRequestParams = Ludeo::create<LudeoSessionAddNotifyPauseGameRequestParams>();

			const LudeoNotificationId NotificationID = ludeo_Session_AddNotifyPauseGameRequest
			(
				SessionHandle,
				&AddNotifyPauseGameRequestParams,
				static_cast<LudeoHSession>(SessionHandle),
				&FLudeoSession::StaticOnPauseGameRequested
			);

			check(NotificationID != LUDEO_INVALID_NOTIFICATIONID);

			NotificationIDCollection.Add(NotificationID);
		}

		// On Resume Game Requested
		{
			const LudeoSessionAddNotifyResumeGameRequestParams AddNotifyResumeGameRequestParams = Ludeo::create<LudeoSessionAddNotifyResumeGameRequestParams>();
			
			const LudeoNotificationId NotificationID = ludeo_Session_AddNotifyResumeGameRequest
			(
				SessionHandle,
				&AddNotifyResumeGameRequestParams,
				static_cast<LudeoHSession>(SessionHandle),
				&FLudeoSession::StaticOnResumeGameRequested
			);

			check(NotificationID != LUDEO_INVALID_NOTIFICATIONID);

			NotificationIDCollection.Add(NotificationID);
		}

		// On Game Back to Menu Requested
		{
			const LudeoSessionAddNotifyBackToMenuRequestParams AddNotifyGameBackToMenuRequestParams = Ludeo::create<LudeoSessionAddNotifyBackToMenuRequestParams>();

			const LudeoNotificationId NotificationID = ludeo_Session_AddNotifyBackToMenuRequest
			(
				SessionHandle,
				&AddNotifyGameBackToMenuRequestParams,
				static_cast<LudeoHSession>(SessionHandle),
				&FLudeoSession::StaticOnGameBackToMenuRequested
			);

			check(NotificationID != LUDEO_INVALID_NOTIFICATIONID);

			NotificationIDCollection.Add(NotificationID);
		}

		// On Room Ready
		{
			const LudeoSessionAddNotifyRoomReadyParams AddNotifyRoomReadyParams = Ludeo::create<LudeoSessionAddNotifyRoomReadyParams>();

			const LudeoNotificationId NotificationID = ludeo_Session_AddNotifyRoomReady
			(
				SessionHandle,
				&AddNotifyRoomReadyParams,
				static_cast<LudeoHSession>(SessionHandle),
				&FLudeoSession::StaticOnRoomReady
			);

			check(NotificationID != LUDEO_INVALID_NOTIFICATIONID);

			NotificationIDCollection.Add(NotificationID);
		}
	}
}

bool FLudeoSession::ReleaseLudeo(const FLudeoHandle& LudeoHandle)
{
	const int32 Index = LudeoCollection.IndexOfByPredicate([&](const FLudeo& Ludeo)
	{
		return (static_cast<FLudeoHandle>(Ludeo) == LudeoHandle);
	});

	if (LudeoCollection.IsValidIndex(Index))
	{
		LudeoCollection.RemoveAtSwap(Index);
	}

	return LudeoCollection.IsValidIndex(Index);
}

void FLudeoSession::StaticOnGetLudeo(const LudeoSessionGetLudeoCallbackParams* pGetLudeoCallbackParams)
{
	check(pGetLudeoCallbackParams != nullptr);
	check(pGetLudeoCallbackParams->clientData != nullptr);

	if (FLudeoSessionPendingGetLudeoData* PendingGetLudeoData = static_cast<FLudeoSessionPendingGetLudeoData*>(pGetLudeoCallbackParams->clientData))
	{
		if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(PendingGetLudeoData->SessionHandle))
		{
			LudeoSession->OnGetLudeo(*pGetLudeoCallbackParams);
		}

		delete PendingGetLudeoData;
	}
}

void FLudeoSession::OnGetLudeo(const LudeoSessionGetLudeoCallbackParams& GetLudeoCallbackParams)
{
	if (FLudeoSessionPendingGetLudeoData* PendingGetLudeoData = static_cast<FLudeoSessionPendingGetLudeoData*>(GetLudeoCallbackParams.clientData))
	{
		const FLudeoResult Result(GetLudeoCallbackParams.resultCode);

		if (Result.IsSuccessful())
		{
			const bool bHasExistingLudeo = LudeoCollection.ContainsByPredicate([&](const FLudeo& Ludeo)
			{
				return (static_cast<FLudeoHandle>(Ludeo) == GetLudeoCallbackParams.dataReader);
			});

			if(!bHasExistingLudeo)
			{
				LudeoCollection.Emplace(GetLudeoCallbackParams.dataReader);
			}
		}

		PendingGetLudeoData->OnGetLudeoDelegate.ExecuteIfBound
		(
			Result,
			SessionHandle,
			GetLudeoCallbackParams.dataReader
		);
	}
}

void FLudeoSession::StaticOnLudeoSelected(const LudeoSessionLudeoSelectedCallbackParams* pLudeoSelectedCallbackParams)
{
	check(pLudeoSelectedCallbackParams != nullptr);
	check(pLudeoSelectedCallbackParams->clientData != nullptr);

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(static_cast<LudeoHSession>(pLudeoSelectedCallbackParams->clientData)))
	{
		LudeoSession->OnLudeoSelected(*pLudeoSelectedCallbackParams);
	}
}

void FLudeoSession::OnLudeoSelected(const LudeoSessionLudeoSelectedCallbackParams& LudeoSelectedCallbackParams) const
{
	OnLudeoSelectedDelegate.Broadcast
	(
		SessionHandle,
		UTF8_TO_TCHAR(LudeoSelectedCallbackParams.ludeoId)
	);
}

void FLudeoSession::StaticOnPauseGameRequested(const LudeoSessionPauseGameRequestCallbackParams* pPauseGameRequestCallbackParams)
{
	check(pPauseGameRequestCallbackParams != nullptr);
	check(pPauseGameRequestCallbackParams->clientData != nullptr);

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(static_cast<LudeoHSession>(pPauseGameRequestCallbackParams->clientData)))
	{
		LudeoSession->OnPauseGameRequested(*pPauseGameRequestCallbackParams);
	}
}

void FLudeoSession::OnPauseGameRequested(const LudeoSessionPauseGameRequestCallbackParams& PauseGameRequestCallbackParams) const
{
	OnPauseGameRequestedDelegate.Broadcast(SessionHandle);
}

void FLudeoSession::StaticOnResumeGameRequested(const LudeoSessionResumeGameRequestCallbackParams* pPauseGameRequestCallbackParams)
{
	check(pPauseGameRequestCallbackParams != nullptr);
	check(pPauseGameRequestCallbackParams->clientData != nullptr);

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(static_cast<LudeoHSession>(pPauseGameRequestCallbackParams->clientData)))
	{
		LudeoSession->OnResumeGameRequested(*pPauseGameRequestCallbackParams);
	}
}

void FLudeoSession::OnResumeGameRequested(const LudeoSessionResumeGameRequestCallbackParams& PauseGameRequestCallbackParams) const
{
	OnResumeGameRequestedDelegate.Broadcast(SessionHandle);
}

void FLudeoSession::StaticOnGameBackToMenuRequested(const LudeoSessionBackToMenuRequestCallbackParams* pGameBackToMenuRequestCallbackParams)
{
	check(pGameBackToMenuRequestCallbackParams != nullptr);
	check(pGameBackToMenuRequestCallbackParams->clientData != nullptr);

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(static_cast<LudeoHSession>(pGameBackToMenuRequestCallbackParams->clientData)))
	{
		LudeoSession->OnGameBackToMenuRequested(*pGameBackToMenuRequestCallbackParams);
	}
}

void FLudeoSession::OnGameBackToMenuRequested(const LudeoSessionBackToMenuRequestCallbackParams& GameBackToMenuRequestCallbackParams) const
{
	OnGameBackToMenuRequestedDelegate.Broadcast(SessionHandle);
}

void FLudeoSession::StaticOnRoomReady(const LudeoSessionRoomReadyCallbackParams* pRoomReadyCallbackParams)
{
	check(pRoomReadyCallbackParams != nullptr);
	check(pRoomReadyCallbackParams->clientData != nullptr);

	if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(static_cast<LudeoHSession>(pRoomReadyCallbackParams->clientData)))
	{
		LudeoSession->OnRoomReady(*pRoomReadyCallbackParams);
	}
}

void FLudeoSession::OnRoomReady(const LudeoSessionRoomReadyCallbackParams& RoomReadyCallbackParams) const
{
	OnRoomReadyDelegate.Broadcast(SessionHandle, RoomReadyCallbackParams.room);
}

void FLudeoSession::StaticOnSessionActivated(const LudeoSessionActivateCallbackParams* pSessionActivateCallbackParams)
{
	check(pSessionActivateCallbackParams != nullptr);
	check(pSessionActivateCallbackParams->clientData != nullptr);

	if (FLudeoSessionPendingActivateSessionData* PendingActivateSessionData = static_cast<FLudeoSessionPendingActivateSessionData*>(pSessionActivateCallbackParams->clientData))
	{
		if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(PendingActivateSessionData->SessionHandle))
		{
			LudeoSession->OnSessionActivated(*pSessionActivateCallbackParams);
		}

		delete PendingActivateSessionData;
	}
}

void FLudeoSession::OnSessionActivated(const LudeoSessionActivateCallbackParams& SessionActivateCallbackParams) const
{
	if (FLudeoSessionPendingActivateSessionData* PendingActivateSessionData = static_cast<FLudeoSessionPendingActivateSessionData*>(SessionActivateCallbackParams.clientData))
	{
		PendingActivateSessionData->OnActivatedDelegate.ExecuteIfBound
		(
			SessionActivateCallbackParams.resultCode,
			SessionHandle,
			(SessionActivateCallbackParams.ludeoSelected == LUDEO_TRUE)
		);
	}
}

void FLudeoSession::OnRoomOpen(const LudeoSessionOpenRoomCallbackParams& OpenRoomCallbackParams)
{
	if (FLudeoSessionPendingOpenRoomData* PendingOpenRoomData = static_cast<FLudeoSessionPendingOpenRoomData*>(OpenRoomCallbackParams.clientData))
	{
		const FLudeoResult Result(OpenRoomCallbackParams.resultCode);

		if (Result.IsSuccessful())
		{
			RoomCollection.Emplace(OpenRoomCallbackParams.room, OpenRoomCallbackParams.dataWriter);
		}

		PendingOpenRoomData->OnOpenRoomDelegate.ExecuteIfBound
		(
			Result,
			SessionHandle,
			OpenRoomCallbackParams.room
		);
	}
}

void FLudeoSession::StaticOnRoomOpen(const LudeoSessionOpenRoomCallbackParams* pOpenRoomCallbackParams)
{
	check(pOpenRoomCallbackParams != nullptr);
	check(pOpenRoomCallbackParams->clientData != nullptr);

	if (FLudeoSessionPendingOpenRoomData* PendingOpenRoomData = static_cast<FLudeoSessionPendingOpenRoomData*>(pOpenRoomCallbackParams->clientData))
	{
		if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(PendingOpenRoomData->SessionHandle))
		{
			LudeoSession->OnRoomOpen(*pOpenRoomCallbackParams);
		}

		delete PendingOpenRoomData;
	}
}

void FLudeoSession::OnRoomClose(const LudeoRoomCloseCallbackParams& CloseRoomCallbackParams)
{
	if (FLudeoSessionPendingCloseRoomData* PendingCloseRoomData = static_cast<FLudeoSessionPendingCloseRoomData*>(CloseRoomCallbackParams.clientData))
	{
		if (FLudeoResult(CloseRoomCallbackParams.resultCode).IsSuccessful())
		{
			const int32 RoomIndex = RoomCollection.IndexOfByPredicate([&](const FLudeoRoom& Room)
			{
				return (static_cast<FLudeoRoomHandle>(Room) == PendingCloseRoomData->RoomHandle);
			});

			check(RoomCollection.IsValidIndex(RoomIndex));
			
			RoomCollection.RemoveAtSwap(RoomIndex);
		}

		PendingCloseRoomData->OnCloseRoomDelegate.ExecuteIfBound
		(
			CloseRoomCallbackParams.resultCode,
			SessionHandle,
			PendingCloseRoomData->RoomHandle
		);
	}
}

void FLudeoSession::StaticOnRoomClose(const LudeoRoomCloseCallbackParams* pCloseRoomCallbackParams)
{
	check(pCloseRoomCallbackParams != nullptr);
	check(pCloseRoomCallbackParams->clientData != nullptr);

	if (FLudeoSessionPendingCloseRoomData* PendingCloseRoomData = static_cast<FLudeoSessionPendingCloseRoomData*>(pCloseRoomCallbackParams->clientData))
	{
		if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(PendingCloseRoomData->SessionHandle))
		{
			LudeoSession->OnRoomClose(*pCloseRoomCallbackParams);
		}

		delete PendingCloseRoomData;
	}
}

FLudeoRoom* FLudeoSession::GetRoomByRoomHandle(const FLudeoRoomHandle& RoomHandle) const
{
	if (RoomHandle != nullptr)
	{
		const int32 RoomIndex = RoomCollection.IndexOfByPredicate([&](const FLudeoRoom& Room)
		{
			return (static_cast<FLudeoRoomHandle>(Room) == RoomHandle);
		});

		if (RoomCollection.IsValidIndex(RoomIndex))
		{
			return const_cast<FLudeoRoom*>(&RoomCollection[RoomIndex]);
		}
	}

	return nullptr;
}

FLudeoSession* FLudeoSession::GetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle)
{
	return FLudeoSessionManager::StaticGetSessionBySessionHandle(SessionHandle);
}

FLudeoRoom* FLudeoSession::GetRoomByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle) const
{
	if (PlayerHandle != nullptr)
	{
		const int32 RoomIndex = RoomCollection.IndexOfByPredicate([&](const FLudeoRoom& Room)
		{
			return (Room.GetPlayerByPlayerHandle(PlayerHandle) != nullptr);
		});

		if (RoomCollection.IsValidIndex(RoomIndex))
		{
			return const_cast<FLudeoRoom*>(&RoomCollection[RoomIndex]);
		}
	}

	return nullptr;
}
