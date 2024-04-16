#include "LudeoSession/LudeoSessionManager.h"

#include "LudeoManager/LudeoManager.h"

struct FPendingDestroyLudeoSessionData
{
	FLudeoSessionHandle SessionHandle;
	FOnLudeoSessionDestroyedDelegate OnLudeoSessionDestroyedDelegate;
};

FLudeoSessionManager* FLudeoSessionManager::GetInstance()
{
	if (const TSharedPtr<FLudeoManager> LudeoManager = FLudeoManager::GetInstance().Pin())
	{
		return &LudeoManager->GetSessionManager();
	}
	
	return nullptr;
}

void FLudeoSessionManager::Finalize()
{
	while (SessionCollection.Num() > 0)
	{
		const LudeoSessionReleaseParams SessionReleaseParams = Ludeo::create<LudeoSessionReleaseParams>();

		const int32 Index = SessionCollection.Num() - 1;

		const FLudeoSessionHandle& SessionHandle = SessionCollection[Index];

		SessionCollection.RemoveAt(Index, 1, false);

		ludeo_Session_Release
		(
			SessionHandle,
			&SessionReleaseParams,
			nullptr,
			[](const LudeoSessionReleaseCallbackParams*)
			{
				// I don't care since this is being shutdown
			}
		);
	}
}

FLudeoSession* FLudeoSessionManager::CreateSession(const FCreateLudeoSessionParameters& CreateLudeoSessionParameters)
{
	const LudeoSessionCreateParams InternalSessionCreateParams = Ludeo::create<LudeoSessionCreateParams>();

	LudeoHSession SessionHandle;
	const FLudeoResult Result = ludeo_Session_Create(&InternalSessionCreateParams, &SessionHandle);

	if (Result.IsSuccessful())
	{
		return &SessionCollection.Emplace_GetRef(SessionHandle);
	}
	
	return nullptr;
}

void FLudeoSessionManager::DestroySession
(
	const FDestroyLudeoSessionParameters& DestroyLudeoSessionParameters,
	const FOnLudeoSessionDestroyedDelegate& OnLudeoSessionDestroyedDelegate
) const
{
	FPendingDestroyLudeoSessionData* PendingDestroyLudeoSessionData = new FPendingDestroyLudeoSessionData;
	PendingDestroyLudeoSessionData->SessionHandle = DestroyLudeoSessionParameters.SessionHandle;
	PendingDestroyLudeoSessionData->OnLudeoSessionDestroyedDelegate = OnLudeoSessionDestroyedDelegate;

	const LudeoSessionReleaseParams SessionReleaseParams = Ludeo::create<LudeoSessionReleaseParams>();

	ludeo_Session_Release
	(
		DestroyLudeoSessionParameters.SessionHandle,
		&SessionReleaseParams,
		PendingDestroyLudeoSessionData,
		&FLudeoSessionManager::StaticOnSessionDestroyed
	);
}

FLudeoSession* FLudeoSessionManager::StaticGetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle)
{
	if (SessionHandle != nullptr)
	{
		if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
		{
			return SessionManager->GetSessionBySessionHandle(SessionHandle);
		}
	}

	return nullptr;
}
FLudeoSession* FLudeoSessionManager::StaticGetSessionByRoomHandle(const FLudeoRoomHandle& RoomHandle)
{
	if (RoomHandle != nullptr)
	{
		if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
		{
			const int32 SessionIndex = SessionManager->SessionCollection.IndexOfByPredicate([&](const FLudeoSession& Session)
			{
				return (Session.GetRoomByRoomHandle(RoomHandle) != nullptr);
			});

			if (SessionManager->SessionCollection.IsValidIndex(SessionIndex))
			{
				return &SessionManager->SessionCollection[SessionIndex];
			}
		}
	}

	return nullptr;
}

FLudeoSession* FLudeoSessionManager::StaticGetSessionByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle)
{
	if (PlayerHandle != nullptr)
	{
		if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
		{
			const int32 SessionIndex = SessionManager->SessionCollection.IndexOfByPredicate([&](const FLudeoSession& Session)
			{
				return (Session.GetRoomByPlayerHandle(PlayerHandle) != nullptr);
			});

			if (SessionManager->SessionCollection.IsValidIndex(SessionIndex))
			{
				return &SessionManager->SessionCollection[SessionIndex];
			}
		}
	}

	return nullptr;
}

FLudeoSession* FLudeoSessionManager::StaticGetSessionByLudeoHandle(const FLudeoHandle& LudeoHandle)
{
	if (LudeoHandle != nullptr)
	{
		if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
		{
			const int32 SessionIndex = SessionManager->SessionCollection.IndexOfByPredicate([&](const FLudeoSession& Session)
			{
				return (Session.GetLudeoByLudeoHandle(LudeoHandle) != nullptr);
			});

			if (SessionManager->SessionCollection.IsValidIndex(SessionIndex))
			{
				return &SessionManager->SessionCollection[SessionIndex];
			}
		}
	}

	return nullptr;
}

FLudeoSession* FLudeoSessionManager::GetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle) const
{
	if (SessionHandle != nullptr)
	{
		const int32 SessionIndex = SessionCollection.IndexOfByPredicate([&](const FLudeoSession& Session)
		{
			return (static_cast<FLudeoSessionHandle>(Session) == SessionHandle);
		});

		if (SessionCollection.IsValidIndex(SessionIndex))
		{
			return const_cast<FLudeoSession*>(&SessionCollection[SessionIndex]);
		}
	}

	return nullptr;
}

void FLudeoSessionManager::StaticOnSessionDestroyed(const LudeoSessionReleaseCallbackParams* pSessionReleaseCallbackParams)
{
	check(pSessionReleaseCallbackParams != nullptr);
	check(pSessionReleaseCallbackParams->clientData != nullptr);

	if (FPendingDestroyLudeoSessionData* PendingDestroyLudeoSessionData = static_cast<FPendingDestroyLudeoSessionData*>(pSessionReleaseCallbackParams->clientData))
	{
		if (FLudeoSessionManager* LudeoSessionManager = FLudeoSessionManager::GetInstance())
		{
			LudeoSessionManager->OnSessionDestroyed(*pSessionReleaseCallbackParams);
		}

		delete PendingDestroyLudeoSessionData;
	}
}

void FLudeoSessionManager::OnSessionDestroyed(const LudeoSessionReleaseCallbackParams& SessionReleaseCallbackParams)
{
	if (FPendingDestroyLudeoSessionData* PendingDestroyLudeoSessionData = static_cast<FPendingDestroyLudeoSessionData*>(SessionReleaseCallbackParams.clientData))
	{
		const FLudeoResult Result(SessionReleaseCallbackParams.resultCode);

		if (Result.IsSuccessful())
		{
			const int32 SessionIndex = SessionCollection.IndexOfByPredicate([&](const FLudeoSession& Session)
			{
				return (static_cast<FLudeoSessionHandle>(Session) == PendingDestroyLudeoSessionData->SessionHandle);
			});

			// It may be deleted by other release call or destructor
			if (SessionCollection.IsValidIndex(SessionIndex))
			{
				SessionCollection.RemoveAtSwap(SessionIndex);
			}
		}

		PendingDestroyLudeoSessionData->OnLudeoSessionDestroyedDelegate.ExecuteIfBound
		(
			Result,
			PendingDestroyLudeoSessionData->SessionHandle
		);
	}
}
