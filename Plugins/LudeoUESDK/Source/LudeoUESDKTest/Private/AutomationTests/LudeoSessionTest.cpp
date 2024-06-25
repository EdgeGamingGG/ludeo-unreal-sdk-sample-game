#include "LudeoTestUtility.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionGetSessionBySessionHandleTest,		"Ludeo.Session.GetSessionBySessionHandle");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionGetLudeoTest,						"Ludeo.Session.GetLudeo");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionActivateTest,						"Ludeo.Session.Activate");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionOpenRoomTest,						"Ludeo.Session.OpenRoom");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionCloseRoomTest,						"Ludeo.Session.CloseRoom");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionGetRoomByRoomHandleTest,			"Ludeo.Session.GetRoomByRoomHandle");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionGetRoomByPlayerHandleTest,			"Ludeo.Session.GetRoomByPlayerHandle");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionIsValidTest,						"Ludeo.Session.IsValid");

bool FLudeoSessionGetLudeoTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_GetLudeo,
			[](LudeoHSession, const LudeoSessionGetLudeoParams*, void* ClientData, LudeoSessionGetLudeoCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionGetLudeoCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;
					CallbackData.dataReader = reinterpret_cast<LudeoHDataReader>(0xDeadBeefull);

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetInfo,
			[](LudeoHDataReader, const LudeoDataReaderGetInfoParams*, LudeoDataReaderInfo** OutDataReaderInformation)
			{
				if (OutDataReaderInformation != nullptr)
				{
					static LudeoDataReaderInfo DataReaderInformation;
					DataReaderInformation.ludeoId = "Bad Beef";
					DataReaderInformation.playerId = "0";

					*OutDataReaderInformation = &DataReaderInformation;
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReaderInfo_Release,
			[](...)
			{

			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetObjectsInfo,
			[](LudeoHDataReader, const LudeoDataReaderGetObjectsInfoParams*, LudeoObjectsInfo** OutObjectsInformation)
			{
				if (OutObjectsInformation != nullptr)
				{
					static LudeoObjectsInfo ObjectsInformation;
					ObjectsInformation.objectsCount = static_cast<uint32_t>(0);
					ObjectsInformation.objects = nullptr;

					*OutObjectsInformation = &ObjectsInformation;
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_ObjectsInfo_Release,
			[](...)
			{

			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoSession* Session = nullptr;

		const FScopedFuctionExecutionGuard Guard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		bool bHasReceivedResult = false;

		Session->GetLudeo
		(
			{},
			FLudeoSessionOnGetLudeoDelegate::CreateLambda([&](const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoHandle& LudeoHandle)
			{
				TEST_LUDEO_RESULT(TEXT("Activate Session"), Result, LudeoResult::Success);
				TestEqual(TEXT("Session Handle"), SessionHandle, static_cast<FLudeoSessionHandle>(*Session));
				TestEqual(TEXT("Session Handle"), static_cast<LudeoHSession>(SessionHandle), reinterpret_cast<LudeoHSession>(0xBadBeefull));
				TestEqual(TEXT("Ludeo Handle"), static_cast<LudeoHDataReader>(LudeoHandle), reinterpret_cast<LudeoHDataReader>(0xDeadBeefull));

				bHasReceivedResult = true;
			})
		);

		TestTrue("Callback Called Flag", bHasReceivedResult);
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionActivateTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Activate,
			[](LudeoHSession, const LudeoSessionActivateParams*, void* ClientData, LudeoSessionActivateCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionActivateCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;
					CallbackData.ludeoSelected = LUDEO_FALSE;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_AddNotifyLudeoSelected,
			[](...)
			{
				return static_cast<LudeoNotificationId>(0xBadBeef);
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_AddNotifyPauseGameRequest,
			[](...)
			{
				return static_cast<LudeoNotificationId>(0xBadBeef);
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_AddNotifyResumeGameRequest,
			[](...)
			{
				return static_cast<LudeoNotificationId>(0xBadBeef);
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_AddNotifyBackToMenuRequest,
			[](...)
			{
				return static_cast<LudeoNotificationId>(0xBadBeef);
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_AddNotifyRoomReady,
			[](...)
			{
				return static_cast<LudeoNotificationId>(0xBadBeef);
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_AddNotifyConsentUpdated,
			[](...)
			{
				return static_cast<LudeoNotificationId>(0xBadBeef);
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoSession* Session = nullptr;

		const FScopedFuctionExecutionGuard Guard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		FLudeoSessionActivateSessionParameters ActivateSessionParameters;
		ActivateSessionParameters.ApiKey				= TEXT("My API Key");
		ActivateSessionParameters.AppToken				= FString();
		ActivateSessionParameters.GameWindowHandle	= nullptr;

		bool bHasReceivedResult = false;

		Session->Activate
		(
			ActivateSessionParameters,
			FLudeoSessionOnActivatedDelegate::CreateLambda
			(
				[&](const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const bool)
				{
					TEST_LUDEO_RESULT(TEXT("Activate Session"), Result, LudeoResult::Success);
					TestEqual(TEXT("Session Handle"), SessionHandle, static_cast<FLudeoSessionHandle>(*Session));

					bHasReceivedResult = true;
				}
			)
		);

		TestTrue("Callback Called Flag", bHasReceivedResult);
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionOpenRoomTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_OpenRoom,
			[](LudeoHSession, const LudeoSessionOpenRoomParams*, void* ClientData, LudeoSessionOpenRoomCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionOpenRoomCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;
					CallbackData.room = reinterpret_cast<LudeoHRoom>(0xDadBeefull);
					CallbackData.dataWriter = reinterpret_cast<LudeoHDataWriter>(0xDeafBeefull);

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_GetInfo,
			[](LudeoHRoom, const LudeoRoomGetInfoParams*, LudeoRoomInfo** OutRoomInformation)
			{
				static LudeoRoomInfo RoomInformation;
				RoomInformation.roomId = "Bad Beef";
				RoomInformation.ludeoId = "Dad Beef";

				if (OutRoomInformation != nullptr)
				{
					*OutRoomInformation = &RoomInformation;
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_RoomInfo_Release,
			[](...)
			{

			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoSession* Session = nullptr;

		const FScopedFuctionExecutionGuard Guard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		FLudeoSessionOpenRoomParameters OpenRoomParameters;
		OpenRoomParameters.LudeoID = FString();
		OpenRoomParameters.RoomID = FString();

		bool bHasReceivedResult = false;

		Session->OpenRoom
		(
			OpenRoomParameters,
			FLudeoSessionOnOpenRoomDelegate::CreateLambda
			(
				[&]
				(
					const FLudeoResult& Result,
					const FLudeoSessionHandle& SessionHandle,
					const FLudeoRoomHandle& RoomHandle
				)
				{
					TEST_LUDEO_RESULT(TEXT("Open Room"), Result, LudeoResult::Success);

					TestEqual(TEXT("Session Handle"), SessionHandle, static_cast<FLudeoSessionHandle>(*Session));
					TestEqual(TEXT("Room Handle"), RoomHandle, FLudeoRoomHandle(reinterpret_cast<LudeoHRoom>(0xDadBeefull)));
					
					bHasReceivedResult = true;
				}
			)
		);

		TestTrue("Callback Called Flag", bHasReceivedResult);
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionCloseRoomTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_OpenRoom,
			[](LudeoHSession, const LudeoSessionOpenRoomParams*, void* ClientData, LudeoSessionOpenRoomCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionOpenRoomCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;
					CallbackData.room = reinterpret_cast<LudeoHRoom>(0xDadBeefull);
					CallbackData.dataWriter = reinterpret_cast<LudeoHDataWriter>(0xDeafBeefull);

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_Close,
			[](LudeoHRoom, const LudeoRoomCloseParams*, void* ClientData, LudeoRoomCloseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoRoomCloseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_GetInfo,
			[](LudeoHRoom, const LudeoRoomGetInfoParams*, LudeoRoomInfo** OutRoomInformation)
			{
				static LudeoRoomInfo RoomInformation;
				RoomInformation.roomId = "Bad Beef";
				RoomInformation.ludeoId = "Dad Beef";

				if (OutRoomInformation != nullptr)
				{
					*OutRoomInformation = &RoomInformation;
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_RoomInfo_Release,
			[](...)
			{

			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoSession* Session = nullptr;

		const FScopedFuctionExecutionGuard SessionGuard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		{
			FLudeoSessionCloseRoomParameters CloseRoomParameters;

			const FScopedFuctionExecutionGuard RoomGuard
			(
				[&]()
				{
					Session->OpenRoom
					(
						{},
						FLudeoSessionOnOpenRoomDelegate::CreateLambda
						(
							[&](const FLudeoResult&, const FLudeoSessionHandle&, const FLudeoRoomHandle& InRoomHandle)
							{
								CloseRoomParameters.RoomHandle = InRoomHandle;
							}
						)
					);
				},
				[&]()
				{
					bool bHasReceivedResult = false;

					Session->CloseRoom
					(
						CloseRoomParameters,
						FLudeoSessionOnCloseRoomDelegate::CreateLambda
						(
							[&](const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoRoomHandle& RoomHandle)
							{
								TEST_LUDEO_RESULT(TEXT("Close Room"), Result, LudeoResult::Success);
								TestEqual(TEXT("Session Handle"), SessionHandle, static_cast<FLudeoSessionHandle>(*Session));
								TestEqual(TEXT("Room Handle"), RoomHandle, CloseRoomParameters.RoomHandle);

								bHasReceivedResult = true;
							}
						)
					);

					TestTrue("Callback Called Flag", bHasReceivedResult);
				}
			);
		}		
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionGetRoomByRoomHandleTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_OpenRoom,
			[](LudeoHSession, const LudeoSessionOpenRoomParams*, void* ClientData, LudeoSessionOpenRoomCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionOpenRoomCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;
					CallbackData.room = reinterpret_cast<LudeoHRoom>(0xDadBeefull);
					CallbackData.dataWriter = reinterpret_cast<LudeoHDataWriter>(0xDeafBeefull);

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_Close,
			[](LudeoHRoom, const LudeoRoomCloseParams*, void* ClientData, LudeoRoomCloseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoRoomCloseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_GetInfo,
			[](LudeoHRoom, const LudeoRoomGetInfoParams*, LudeoRoomInfo** OutRoomInformation)
			{
				static LudeoRoomInfo RoomInformation;
				RoomInformation.roomId = "Bad Beef";
				RoomInformation.ludeoId = "Dad Beef";

				if (OutRoomInformation != nullptr)
				{
					*OutRoomInformation = &RoomInformation;
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_RoomInfo_Release,
			[](...)
			{

			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoSession* Session = nullptr;

		const FScopedFuctionExecutionGuard SessionGuard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		// Room Guard
		{
			FLudeoRoom* Room = nullptr;

			const FScopedFuctionExecutionGuard RoomGuard
			(
				[&]()
				{
					Room = Session->GetRoomByRoomHandle(FLudeoRoomHandle(reinterpret_cast<LudeoHRoom>(0xDadBeefull)));
					TestNull(TEXT("Room"), Room);

					Session->OpenRoom
					(
						{},
						FLudeoSessionOnOpenRoomDelegate::CreateLambda
						(
							[&]
							(
								const FLudeoResult&,
								const FLudeoSessionHandle&,
								const FLudeoRoomHandle& RoomHandle
							)
							{
								Room = Session->GetRoomByRoomHandle(RoomHandle);
							}
						)
					);
	
					TestNotNull(TEXT("Room"), Room);
				},
				[&]()
				{
					FLudeoSessionCloseRoomParameters CloseRoomParameters;
					CloseRoomParameters.RoomHandle = *Room;

					Session->CloseRoom(CloseRoomParameters);
					
					Room = Session->GetRoomByRoomHandle(CloseRoomParameters.RoomHandle);
					TestNull(TEXT("Room"), Room);
				}
			);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionGetRoomByPlayerHandleTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_OpenRoom,
			[](LudeoHSession, const LudeoSessionOpenRoomParams*, void* ClientData, LudeoSessionOpenRoomCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionOpenRoomCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;
					CallbackData.room = reinterpret_cast<LudeoHRoom>(0xDadBeefull);
					CallbackData.dataWriter = reinterpret_cast<LudeoHDataWriter>(0xDeafBeefull);

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_GetInfo,
			[](LudeoHRoom, const LudeoRoomGetInfoParams*, LudeoRoomInfo** OutRoomInformation)
			{
				static LudeoRoomInfo RoomInformation;
				RoomInformation.roomId = "Bad Beef";
				RoomInformation.ludeoId = "Dad Beef";

				if (OutRoomInformation != nullptr)
				{
					*OutRoomInformation = &RoomInformation;
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_RoomInfo_Release,
			[](...)
			{

			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_AddPlayer,
			[](LudeoHRoom, const LudeoRoomAddPlayerParams*, void* ClientData, LudeoRoomAddPlayerCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoRoomAddPlayerCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;
					CallbackData.gameplaySession = reinterpret_cast<LudeoHGameplaySession>(0xBadBeefull);

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Room_RemovePlayer,
			[](LudeoHRoom, const LudeoRoomRemovePlayerParams*, void* ClientData, LudeoRoomRemovePlayerCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoRoomRemovePlayerCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_GameplaySession_GetInfo,
			[](LudeoHGameplaySession, const LudeoGameplaySessionGetInfoParams*, LudeoGameplaySessionInfo** OutGameplaySessionInformation)
			{
				if (OutGameplaySessionInformation != nullptr)
				{
					static LudeoGameplaySessionInfo GameplaySessionInfo;
					GameplaySessionInfo.playerId = "Bad Beef";

					*OutGameplaySessionInformation = &GameplaySessionInfo;
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_GameplaySessionInfo_Release,
			[](...)
			{

			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoSession* Session = nullptr;

		const FScopedFuctionExecutionGuard SessionAndRoomGuard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		// Room Guard
		{

			FLudeoRoom* Room = nullptr;

			const FScopedFuctionExecutionGuard RoomGuard
			(
				[&]()
				{
					Session->OpenRoom
					(
						{},
						FLudeoSessionOnOpenRoomDelegate::CreateLambda
						(
							[&]
							(
								const FLudeoResult&,
								const FLudeoSessionHandle&,
								const FLudeoRoomHandle& RoomHandle
							)
							{
								Room = Session->GetRoomByRoomHandle(RoomHandle);
							}
						)
					);
				},
				[&]()
				{

				}
			);

			FLudeoPlayerHandle PlayerHandle(reinterpret_cast<LudeoHGameplaySession>(0xDadBeefull));

			TestNull(TEXT("Room"), Session->GetRoomByPlayerHandle(PlayerHandle));

			FLudeoRoomAddPlayerParameters AddPlayerParameters;
			AddPlayerParameters.PlayerID = TEXT("Bad Beef");

			Room->AddPlayer
			(
				AddPlayerParameters,
				FLudeoRoomOnAddPlayerDelegate::CreateLambda([&](const FLudeoResult&, const FLudeoRoomHandle&, const FLudeoPlayerHandle& InPlayerHandle)
				{
					PlayerHandle = InPlayerHandle;
				})
			);

			Room = Session->GetRoomByPlayerHandle(PlayerHandle);
			TestNotNull(TEXT("Room"), Room);
			
			FLudeoRoomRemovePlayerParameters RemovePlayerParameters;
			RemovePlayerParameters.PlayerID = AddPlayerParameters.PlayerID;

			Room->RemovePlayer(RemovePlayerParameters);

			Room = Session->GetRoomByPlayerHandle(PlayerHandle);
			TestNull(TEXT("Room"), Room);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionGetSessionBySessionHandleTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoSessionHandle SessionHandle(reinterpret_cast<LudeoHSession>(0xBadBeefull));

		{
			const FScopedFuctionExecutionGuard SessionGuard
			(
				[&]()
				{	
					TestNull(TEXT("Session"), FLudeoSession::GetSessionBySessionHandle(SessionHandle));

					const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
					SessionHandle = *FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);

					TestNotNull(TEXT("Session"), FLudeoSession::GetSessionBySessionHandle(SessionHandle));
				},
				[&]()
				{
					FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
					DestroyLudeoSessionParameters.SessionHandle = SessionHandle;

					FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);

					TestNull(TEXT("Session"), FLudeoSession::GetSessionBySessionHandle(SessionHandle));
				}
			);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionIsValidTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams*, LudeoHSession* OutSessionHandle)
			{
				if (OutSessionHandle != nullptr)
				{
					*OutSessionHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
				}

				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Release,
			[](LudeoHSession, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoSessionReleaseCallbackParams CallbackData;
					CallbackData.resultCode = LudeoResult::Success;
					CallbackData.clientData = ClientData;

					Callback(&CallbackData);
				}
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoSession Session(reinterpret_cast<LudeoHSession>(0xBadBeefull));
		const FLudeoSession* pSession = &Session;

		const FScopedFuctionExecutionGuard SessionGuard
		(
			[&]()
			{
				TestFalse(TEXT("Session Validity"), pSession->IsValid());

				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				pSession = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);

				TestTrue(TEXT("Session Validity"), pSession->IsValid());
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *pSession;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);

				TestFalse(TEXT("Session Validity"), pSession->IsValid());
			}
		);
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}
#endif //WITH_DEV_AUTOMATION_TESTS
