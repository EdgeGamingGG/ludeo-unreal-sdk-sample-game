#include "LudeoTestUtility.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionManagerCreateSessionTest,				"Ludeo.SessionManager.CreateSession");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionManagerDestroySessionTest,				"Ludeo.SessionManager.DestroySession");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoSessionManagerGetSessionBySessionHandleTest,	"Ludeo.SessionManager.GetSessionBySessionHandle");

bool FLudeoSessionManagerCreateSessionTest::RunTest(const FString& Parameters)
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
		FLudeoSession* Session = nullptr;

		const FScopedFuctionExecutionGuard Guard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				Session = FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);

				TestNotNull(TEXT("Session"), Session);
				TestEqual
				(
					TEXT("Session Handle"),
					static_cast<FLudeoSessionHandle>(*Session),
					FLudeoSessionHandle(reinterpret_cast<LudeoHSession>(0xBadBeefull))
				);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *Session;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);
			}
		);
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionManagerDestroySessionTest::RunTest(const FString& Parameters)
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
			[](LudeoHSession SessionHandle, const LudeoSessionReleaseParams*, void* ClientData, LudeoSessionReleaseCallback Callback)
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
		FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;

		const FScopedFuctionExecutionGuard Guard
		(
			[&]()
			{
				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = *FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);
			},
			[&]()
			{
				TestNotNull
				(
					TEXT("Session"),
					FLudeoSessionManager::GetInstance()->GetSessionBySessionHandle(DestroyLudeoSessionParameters.SessionHandle)
				);

				bool bHasReceivedResult = false;

				FLudeoSessionManager::GetInstance()->DestroySession
				(
					DestroyLudeoSessionParameters,
					FOnLudeoSessionDestroyedDelegate::CreateLambda([&](const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle)
					{
						TEST_LUDEO_RESULT(TEXT("Destroy Session"), Result, LudeoResult::Success);
						TestEqual(TEXT("Session Handle"), SessionHandle, DestroyLudeoSessionParameters.SessionHandle);

						bHasReceivedResult = true;
					})
				);

				TestTrue("Callback Called Flag", bHasReceivedResult);

				TestNull
				(
					TEXT("Session"),
					FLudeoSessionManager::GetInstance()->GetSessionBySessionHandle(DestroyLudeoSessionParameters.SessionHandle)
				);
			}
		);		
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoSessionManagerGetSessionBySessionHandleTest::RunTest(const FString& Parameters)
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

		const FScopedFuctionExecutionGuard Guard
		(
			[&]()
			{
				TestNull
				(
					TEXT("Session"),
					FLudeoSessionManager::GetInstance()->GetSessionBySessionHandle(SessionHandle)
				);

				const FCreateLudeoSessionParameters CreateLudeoSessionParameters;
				SessionHandle = *FLudeoSessionManager::GetInstance()->CreateSession(CreateLudeoSessionParameters);

				TestNotNull
				(
					TEXT("Session"),
					FLudeoSessionManager::GetInstance()->GetSessionBySessionHandle(SessionHandle)
				);
			},
			[&]()
			{
				FDestroyLudeoSessionParameters DestroyLudeoSessionParameters;
				DestroyLudeoSessionParameters.SessionHandle = SessionHandle;

				FLudeoSessionManager::GetInstance()->DestroySession(DestroyLudeoSessionParameters);

				TestNull
				(
					TEXT("Session"),
					FLudeoSessionManager::GetInstance()->GetSessionBySessionHandle(SessionHandle)
				);
			}
		);
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}
#endif //WITH_DEV_AUTOMATION_TESTS
