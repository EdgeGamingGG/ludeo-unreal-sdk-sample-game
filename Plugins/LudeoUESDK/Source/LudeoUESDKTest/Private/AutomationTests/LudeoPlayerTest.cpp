#include "LudeoTestUtility.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoPlayerGetPlayerByPlayerHandleTest,	"Ludeo.Player.GetPlayerByPlayerHandleTest");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoPlayerIsValidTest,					"Ludeo.Player.IsValid");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoPlayerBeginGameplayTest,				"Ludeo.Player.BeginGameplay");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoPlayerEndGameplayTest,				"Ludeo.Player.EndGameplay");

bool FLudeoPlayerGetPlayerByPlayerHandleTest::RunTest(const FString& Parameters)
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
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters{};
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters{};
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

			FLudeoPlayerHandle PlayerHandle(reinterpret_cast<LudeoHGameplaySession>(0xBadBeefull));

			TestNull(TEXT("Player"), FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle));

			FLudeoRoomAddPlayerParameters AddPlayerParameters{};
			AddPlayerParameters.PlayerID = TEXT("Bad Beef");

			Room->AddPlayer
			(
				AddPlayerParameters,
				FLudeoRoomOnAddPlayerDelegate::CreateLambda
				(
					[&](const FLudeoResult&, const FLudeoRoomHandle&, const FLudeoPlayerHandle& InPlayerHandle)
					{
						PlayerHandle = InPlayerHandle;
					}
				)
			);

			const FLudeoPlayer* LudeoPlayer = FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle);
			TestNotNull(TEXT("Player"), LudeoPlayer);

			if(LudeoPlayer != nullptr)
			{
				FLudeoRoomRemovePlayerParameters RemovePlayerParameters;
				RemovePlayerParameters.PlayerID = LudeoPlayer->GetPlayerID();

				Room->RemovePlayer(RemovePlayerParameters);

				TestNull(TEXT("Player"), FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle));
			}
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoPlayerIsValidTest::RunTest(const FString& Parameters)
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
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters{};
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters{};
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

			FLudeoPlayer Player(reinterpret_cast<LudeoHGameplaySession>(0xBadBeefull));

			const FLudeoPlayer* pPlayer = &Player;

			TestFalse(TEXT("Player"), pPlayer->IsValid());

			FLudeoRoomAddPlayerParameters AddPlayerParameters{};
			AddPlayerParameters.PlayerID = TEXT("Bad Beef");

			Room->AddPlayer
			(
				AddPlayerParameters,
				FLudeoRoomOnAddPlayerDelegate::CreateLambda
				(
					[&](const FLudeoResult&, const FLudeoRoomHandle&, const FLudeoPlayerHandle& PlayerHandle)
					{
						pPlayer = FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle);
					}
				)
			);

			if(pPlayer != nullptr)
			{
				TestTrue(TEXT("Player"), pPlayer->IsValid());

				FLudeoRoomRemovePlayerParameters RemovePlayerParameters;
				RemovePlayerParameters.PlayerID = pPlayer->GetPlayerID();

				Room->RemovePlayer(RemovePlayerParameters);

				TestFalse(TEXT("Player"), pPlayer->IsValid());
			}
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoPlayerBeginGameplayTest::RunTest(const FString& Parameters)
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
			ludeo_GameplaySession_Begin,
			[](LudeoHGameplaySession, LudeoGameplaySessionBeginParams*, void* ClientData, LudeoGameplaySessionBeginCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoGameplaySessionBeginCallbackParams CallbackData;
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
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters{};
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters{};
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		{
			FLudeoRoom* Room = nullptr;
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

			FLudeoRoomAddPlayerParameters AddPlayerParameters{};
			AddPlayerParameters.PlayerID = TEXT("Dad Beef");


			const FLudeoPlayer* Player = nullptr;
			Room->AddPlayer
			(
				AddPlayerParameters,
				FLudeoRoomOnAddPlayerDelegate::CreateLambda
				(
					[&](const FLudeoResult&, const FLudeoRoomHandle&, const FLudeoPlayerHandle& PlayerHandle)
					{
						Player = Room->GetPlayerByPlayerHandle(PlayerHandle);
					}
				)
			);

			bool bHasReceivedResult = false;

			Player->BeginGameplay
			(
				{},
				FLudeoPlayerOnBeginGameplayDelegate::CreateLambda
				(
					[&](const FLudeoResult& Result, const FLudeoPlayerHandle& PlayerHandle)
					{
						TEST_LUDEO_RESULT(TEXT("Player Begin Gameplay"), Result, LudeoResult::Success);

						TestEqual(TEXT("Player Handle"), PlayerHandle, static_cast<FLudeoPlayerHandle>(*Player));

						bHasReceivedResult = true;
					}
				)
			);

			TestTrue("Callback Called Flag", bHasReceivedResult);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoPlayerEndGameplayTest::RunTest(const FString& Parameters)
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
			ludeo_GameplaySession_End,
			[](LudeoHGameplaySession, LudeoGameplaySessionEndParams*, void* ClientData, LudeoGameplaySessionEndCallback Callback)
			{
				if (Callback != nullptr)
				{
					LudeoGameplaySessionEndCallbackParams CallbackData;
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
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters{};
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters{};
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);

		{
			FLudeoRoom* Room = nullptr;
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

			FLudeoRoomAddPlayerParameters AddPlayerParameters{};
			AddPlayerParameters.PlayerID = TEXT("Dad Beef");


			const FLudeoPlayer* Player = nullptr;
			Room->AddPlayer
			(
				AddPlayerParameters,
				FLudeoRoomOnAddPlayerDelegate::CreateLambda
				(
					[&](const FLudeoResult&, const FLudeoRoomHandle&, const FLudeoPlayerHandle& PlayerHandle)
					{
						Player = Room->GetPlayerByPlayerHandle(PlayerHandle);
					}
				)
			);

			bool bHasReceivedResult = false;

			Player->EndGameplay
			(
				{},
				FLudeoPlayerOnEndGameplayDelegate::CreateLambda
				(
					[&](const FLudeoResult& Result, const FLudeoPlayerHandle& PlayerHandle)
					{
						TEST_LUDEO_RESULT(TEXT("Player Begin Gameplay"), Result, LudeoResult::Success);

						TestEqual(TEXT("Player Handle"), PlayerHandle, static_cast<FLudeoPlayerHandle>(*Player));

						bHasReceivedResult = true;
					}
				)
			);

			TestTrue("Callback Called Flag", bHasReceivedResult);
		}
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}
#endif