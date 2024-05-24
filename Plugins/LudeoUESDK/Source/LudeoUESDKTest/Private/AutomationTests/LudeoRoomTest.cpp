#include "LudeoTestUtility.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoRoomGetRoomByRoomHandleTest,	"Ludeo.Room.GetRoomByRoomHandle");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoRoomAddPlayerTest,			"Ludeo.Room.AddPlayer");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoRoomRemovePlayerTest,			"Ludeo.Room.RemovePlayer");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoRoomIsValidTest,				"Ludeo.Room.IsValid");

bool FLudeoRoomGetRoomByRoomHandleTest::RunTest(const FString& Parameters)
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
			FLudeoRoomHandle RoomHandle(reinterpret_cast<LudeoHRoom>(0xBadBeefull));

			const FScopedFuctionExecutionGuard RoomGuard
			(
				[&]()
				{
					TestNull(TEXT("Room"), FLudeoRoom::GetRoomByRoomHandle(RoomHandle));

					Session->OpenRoom
					(
						{},
						FLudeoSessionOnOpenRoomDelegate::CreateLambda
						(
							[&](const FLudeoResult&, const FLudeoSessionHandle&, const FLudeoRoomHandle& InRoomHandle)
							{
								RoomHandle = InRoomHandle;
							}
						)
					);

					TestNotNull(TEXT("Room"), FLudeoRoom::GetRoomByRoomHandle(RoomHandle));
				},
				[&]()
				{
					FLudeoSessionCloseRoomParameters CloseRoomParameters;
					CloseRoomParameters.RoomHandle = RoomHandle;

					Session->CloseRoom(CloseRoomParameters);
					

					TestNull(TEXT("Room"), FLudeoRoom::GetRoomByRoomHandle(RoomHandle));
				}
			);
		}		
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoRoomAddPlayerTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams* params, LudeoHSession* outHandle)
			{
				if (outHandle != nullptr)
				{
					*outHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
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
					CallbackData.gameplaySession = reinterpret_cast<LudeoHGameplaySession>(0xDeadBeefull);

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
							[&](const FLudeoResult&, const FLudeoSessionHandle&, const FLudeoRoomHandle& RoomHandle)
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

			FLudeoRoomAddPlayerParameters AddPlayerParameters;
			AddPlayerParameters.PlayerID = TEXT("Dad Beef");

			bool bHasReceivedResult = false;

			Room->AddPlayer
			(
				AddPlayerParameters,
				FLudeoRoomOnAddPlayerDelegate::CreateLambda
				(
					[&](const FLudeoResult& Result, const FLudeoRoomHandle& RoomHandle, const FLudeoPlayerHandle& PlayerHandle)
					{
						TEST_LUDEO_RESULT(TEXT("Add Player"), Result, LudeoResult::Success);

						TestEqual(TEXT("Room Handle"), RoomHandle, FLudeoRoomHandle(reinterpret_cast<LudeoHRoom>(0xDadBeefull)));

						TestNotNull(TEXT("Player"), FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle));

						bHasReceivedResult = true;
					}
				)
			);

			TestTrue("Callback Called Flag", bHasReceivedResult);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoRoomRemovePlayerTest::RunTest(const FString& Parameters)
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
					CallbackData.gameplaySession = reinterpret_cast<LudeoHGameplaySession>(0xDeadBeefull);

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
							[&](const FLudeoResult&, const FLudeoSessionHandle&, const FLudeoRoomHandle& RoomHandle)
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

			FLudeoRoomAddPlayerParameters AddPlayerParameters;
			AddPlayerParameters.PlayerID = TEXT("Bad Beef");

			FLudeoRoomRemovePlayerParameters RemovePlayerParameters;

			Room->AddPlayer
			(
				AddPlayerParameters,
				FLudeoRoomOnAddPlayerDelegate::CreateLambda
				(
					[&](const FLudeoResult&, const FLudeoRoomHandle&, const FLudeoPlayerHandle&)
					{
						RemovePlayerParameters.PlayerID = AddPlayerParameters.PlayerID;
					}
				)
			);

			bool bHasReceivedResult = false;

			Room->RemovePlayer
			(
				RemovePlayerParameters,
				FLudeoRoomOnRemovePlayerDelegate::CreateLambda
				(
					[&](const FLudeoResult& Result, const FLudeoRoomHandle& RoomHandle, const FString& PlayerID)
					{
						TEST_LUDEO_RESULT(TEXT("Remove Player"), Result, LudeoResult::Success);

						TestEqual(TEXT("Room Handle"), RoomHandle, FLudeoRoomHandle(reinterpret_cast<LudeoHRoom>(0xDadBeefull)));
						TestEqual(TEXT("Player Handle"), PlayerID, RemovePlayerParameters.PlayerID);

						bHasReceivedResult = true;
					}
				)
			);

			TestTrue("Callback Called Flag", bHasReceivedResult);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoRoomIsValidTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_Session_Create,
			[](const LudeoSessionCreateParams* params, LudeoHSession* outHandle)
			{
				if (outHandle != nullptr)
				{
					*outHandle = reinterpret_cast<LudeoHSession>(0xBadBeefull);
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
			const FLudeoRoom* pRoom = nullptr;

			const FScopedFuctionExecutionGuard RoomGuard
			(
				[&]()
				{
					const FLudeoRoom Room(reinterpret_cast<LudeoHRoom>(0xBadBeefull), reinterpret_cast<LudeoHDataWriter>(0xDadBeefull));
					pRoom = &Room;

					TestFalse(TEXT("Room validity"), pRoom->IsValid());

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
								pRoom = Session->GetRoomByRoomHandle(RoomHandle);								
							}
						)
					);

					TestTrue(TEXT("Room validity"), pRoom->IsValid());
				},
				[&]()
				{
					FLudeoSessionCloseRoomParameters CloseRoomParameters;
					CloseRoomParameters.RoomHandle = *pRoom;

					Session->CloseRoom(CloseRoomParameters);

					TestFalse(TEXT("Room validity"), pRoom->IsValid());
				}
			);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}
#endif //WITH_DEV_AUTOMATION_TESTS
