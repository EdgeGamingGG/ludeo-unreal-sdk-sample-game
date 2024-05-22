#include "LudeoUESDK/LudeoSession/LudeoSession.h"

#include <Ludeo/Session.h>
#include <Ludeo/DataReader.h>
#include <Ludeo/Room.h>
#include <Ludeo/TemporaryAPI.h>
#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoCallback/LudeoCallbackManager.h"
#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

FLudeoSession::FLudeoSession(const FLudeoSessionHandle& InSessionHandle) :
	SessionHandle(InSessionHandle)
{
	check(SessionHandle != nullptr);
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

	ludeo_Session_GetLudeo
	(
		SessionHandle,
		&GetLudeoParameters,
		FLudeoCallbackManager::GetInstance().CreateCallback
		(
			[
				SessionHandle = SessionHandle,
				LudeoID,
				OnGetLudeoDelegate
			]
			(const LudeoSessionGetLudeoCallbackParams& GetLudeoCallbackParams)
			{
				if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
				{
					const FLudeoResult Result(GetLudeoCallbackParams.resultCode);

					if (Result.IsSuccessful())
					{
						const bool bHasExistingLudeo = LudeoSession->LudeoCollection.ContainsByPredicate([&](const FLudeo& Ludeo)
						{
							return (static_cast<FLudeoHandle>(Ludeo) == GetLudeoCallbackParams.dataReader);
						});

						if (!bHasExistingLudeo)
						{
							LudeoSession->LudeoCollection.Emplace(GetLudeoCallbackParams.dataReader);
						}
					}

					OnGetLudeoDelegate.ExecuteIfBound
					(
						Result,
						SessionHandle,
						GetLudeoCallbackParams.dataReader
					);
				}
			}
		),
		[](const LudeoSessionGetLudeoCallbackParams* pGetLudeoCallbackParams)
		{
			FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pGetLudeoCallbackParams);
		}
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
	FString OverridePlatformURLString;
	FParse::Value(FCommandLine::Get(), TEXT("PlatformURL="), OverridePlatformURLString);

	const FTCHARToUTF8 OverridePlatformURLConverter
	(
		OverridePlatformURLString.GetCharArray().GetData(),
		OverridePlatformURLString.GetCharArray().Num()
	);

	if(OverridePlatformURLString.Len() > 0)
	{
 		InternalSessionActivateParams.platformUrl = OverridePlatformURLConverter.Get();
	}
#endif

 	InternalSessionActivateParams.apiKey = APIKeyStringConverter.Get();
 	InternalSessionActivateParams.appToken = AppTokenStringConverter.Get();
	InternalSessionActivateParams.windowHandle = ActivateSessionParameters.GameWindowHandle;
	InternalSessionActivateParams.reset = static_cast<LudeoBool>(ActivateSessionParameters.bResetAttributeAndAction);

	LudeoSteamAuthDetails SteamAuthDetails;

	if(
		ActivateSessionParameters.AuthenticationType == ELudeoSessionAuthenticationType::Steam &&
		!ActivateSessionParameters.SteamAuthenticationDetails.AuthenticationID.IsEmpty() &&
		!ActivateSessionParameters.SteamAuthenticationDetails.DisplayName.IsEmpty()
	)
	{
		const FTCHARToUTF8 SteamAuthenticationIDStringConverter
		(
			ActivateSessionParameters.SteamAuthenticationDetails.AuthenticationID.GetCharArray().GetData(),
			ActivateSessionParameters.SteamAuthenticationDetails.AuthenticationID.GetCharArray().Num()
		);

		const FTCHARToUTF8 SteamDisplayNameStringConverter
		(
			ActivateSessionParameters.SteamAuthenticationDetails.DisplayName.GetCharArray().GetData(),
			ActivateSessionParameters.SteamAuthenticationDetails.DisplayName.GetCharArray().Num()
		);

		const FTCHARToUTF8 SteamBetaBranchStringConverter
		(
			ActivateSessionParameters.SteamAuthenticationDetails.BetaBranchName.GetCharArray().GetData(),
			ActivateSessionParameters.SteamAuthenticationDetails.BetaBranchName.GetCharArray().Num()
		);

		
		SteamAuthDetails.authType = LudeoAuthType::Steam;
		SteamAuthDetails.authId = SteamAuthenticationIDStringConverter.Get();
		SteamAuthDetails.displayName = SteamDisplayNameStringConverter.Get();
		SteamAuthDetails.currentBetaName = SteamBetaBranchStringConverter.Get();

		InternalSessionActivateParams.authDetails = &SteamAuthDetails;
	}
	
	ludeo_Session_Activate
	(
		SessionHandle,
		&InternalSessionActivateParams,
		FLudeoCallbackManager::GetInstance().CreateCallback
		(
			[
				SessionHandle = SessionHandle,
				OnActivatedDelegate
			]
			(const LudeoSessionActivateCallbackParams& SessionActivateCallbackParams)
			{
				if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
				{
					OnActivatedDelegate.ExecuteIfBound
					(
						SessionActivateCallbackParams.resultCode,
						SessionHandle,
						(SessionActivateCallbackParams.ludeoSelected == LUDEO_TRUE)
					);
				}
			}
		),
		[](const LudeoSessionActivateCallbackParams* pSessionActivateCallbackParams)
		{
			FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pSessionActivateCallbackParams);
		}
	);
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

	ludeo_Session_OpenRoom
	(
		SessionHandle,
		&IntenralOpenRoomParams,
		FLudeoCallbackManager::GetInstance().CreateCallback
		(
			[
				SessionHandle = SessionHandle,
				OnOpenRoomDelegate
			]
			(const LudeoSessionOpenRoomCallbackParams& OpenRoomCallbackParams)
			{
				if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
				{
					const FLudeoResult Result(OpenRoomCallbackParams.resultCode);

					if (Result.IsSuccessful())
					{
						LudeoSession->RoomCollection.Emplace(OpenRoomCallbackParams.room, OpenRoomCallbackParams.dataWriter);
					}

					OnOpenRoomDelegate.ExecuteIfBound
					(
						Result,
						SessionHandle,
						OpenRoomCallbackParams.room
					);
				}
			}
		),
		[](const LudeoSessionOpenRoomCallbackParams* pOpenRoomCallbackParams)
		{
			FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pOpenRoomCallbackParams);
		}
	);
}

void FLudeoSession::CloseRoom
(
	const FLudeoSessionCloseRoomParameters& CloseRoomParameters,
	const FLudeoSessionOnCloseRoomDelegate& OnCloseRoomDelegate
) const
{
	LudeoRoomCloseParams IntenralCloseRoomParams = Ludeo::create<LudeoRoomCloseParams>();

	ludeo_Room_Close
	(
		CloseRoomParameters.RoomHandle,
		&IntenralCloseRoomParams,
		FLudeoCallbackManager::GetInstance().CreateCallback
		(
			[
				SessionHandle = SessionHandle,
				RoomHandle = CloseRoomParameters.RoomHandle,
				OnCloseRoomDelegate
			]
			(const LudeoRoomCloseCallbackParams& CloseRoomCallbackParams)
			{
				if (FLudeoSession* LudeoSession = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
				{
					if (FLudeoResult(CloseRoomCallbackParams.resultCode).IsSuccessful())
					{
						const int32 RoomIndex = LudeoSession->RoomCollection.IndexOfByPredicate([&](const FLudeoRoom& Room)
						{
							return (static_cast<FLudeoRoomHandle>(Room) == RoomHandle);
						});

						check(LudeoSession->RoomCollection.IsValidIndex(RoomIndex));

						if(LudeoSession->RoomCollection.IsValidIndex(RoomIndex))
						{
							LudeoSession->RoomCollection.RemoveAtSwap(RoomIndex);
						}
					}

					OnCloseRoomDelegate.ExecuteIfBound
					(
						CloseRoomCallbackParams.resultCode,
						SessionHandle,
						RoomHandle
					);
				}
			}
		),
		[](const LudeoRoomCloseCallbackParams* pCloseRoomCallbackParams)
		{
			FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pCloseRoomCallbackParams);
		}
	);
}

FLudeoResult FLudeoSession::OpenGallery(const FLudeoSessionOpenGalleryParameters& OpenGalleryParameters) const
{
	const LudeoSessionOpenGalleryParams IntenralOpenGalleryParams = Ludeo::create<LudeoSessionOpenGalleryParams>();

	return ludeo_Session_OpenGallery(SessionHandle, &IntenralOpenGalleryParams);
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
