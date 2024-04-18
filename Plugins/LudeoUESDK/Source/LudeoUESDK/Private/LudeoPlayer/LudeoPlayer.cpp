#include "LudeoPlayer/LudeoPlayer.h"

#include <Ludeo/Utils.h>

#include "LudeoSession/LudeoSessionManager.h"
#include "LudeoRoom/LudeoRoom.h"

struct FLudeoPlayerPendingData
{
	FLudeoPlayerHandle PlayerHandle;
};

struct FLudeoPlayerPendingBeginGameplayData : public FLudeoPlayerPendingData
{
	FLudeoPlayerOnBeginGameplayDelegate OnBeginGameplayDelegate;
};

struct FLudeoPlayerPendingEndGameplayData : public FLudeoPlayerPendingData
{
	FLudeoPlayerOnEndGameplayDelegate OnEndGameplayDelegate;
};

FLudeoPlayer::FLudeoPlayer(const FLudeoPlayerHandle& InPlayerHandle) :
	PlayerHandle(InPlayerHandle)
{
	if(PlayerHandle != nullptr)
	{
		LudeoGameplaySessionInfo* GameplaySessionInformation;

		LudeoGameplaySessionGetInfoParams InternalGetGameplaySessionInformationParams = Ludeo::create<LudeoGameplaySessionGetInfoParams>();

		const FLudeoResult Result = ludeo_GameplaySession_GetInfo
		(
			PlayerHandle,
			&InternalGetGameplaySessionInformationParams,
			&GameplaySessionInformation
		);
		check(Result.IsSuccessful());

		if (Result.IsSuccessful())
		{
			PlayerID = UTF8_TO_TCHAR(GameplaySessionInformation->playerId);

			ludeo_GameplaySessionInfo_Release(GameplaySessionInformation);
		}
	}
}

FLudeoPlayer::~FLudeoPlayer()
{
}

const FLudeoPlayer* FLudeoPlayer::GetPlayerByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle)
{
	if (FLudeoSession* Session = FLudeoSessionManager::StaticGetSessionByPlayerHandle(PlayerHandle))
	{
		if (FLudeoRoom* Room = Session->GetRoomByPlayerHandle(PlayerHandle))
		{
			return Room->GetPlayerByPlayerHandle(PlayerHandle);
		}
	}

	return nullptr;
}

const FString& FLudeoPlayer::GetPlayerID() const
{
	return PlayerID;
}

void FLudeoPlayer::BeginGameplay
(
	const FLudeoPlayerBeginGameplayParameters& BeginGameplayParameters,
	const FLudeoPlayerOnBeginGameplayDelegate& OnBeginGameplayDelegate
) const
{
	LudeoGameplaySessionBeginParams BeginGameplayParams = Ludeo::create<LudeoGameplaySessionBeginParams>();

	FLudeoPlayerPendingBeginGameplayData* PendingBeginGameplayData = new FLudeoPlayerPendingBeginGameplayData;
	PendingBeginGameplayData->OnBeginGameplayDelegate = OnBeginGameplayDelegate;
	PendingBeginGameplayData->PlayerHandle = PlayerHandle;

	ludeo_GameplaySession_Begin(PlayerHandle, &BeginGameplayParams, PendingBeginGameplayData, &FLudeoPlayer::StaticOnBeginGameplay);
}

void FLudeoPlayer::EndGameplay
(
	const FLudeoPlayerEndGameplayParameters& EndGameplayParameters,
	const FLudeoPlayerOnEndGameplayDelegate& OnEndGameplayDelegate
) const
{
	LudeoGameplaySessionEndParams EndGameplayParams = Ludeo::create<LudeoGameplaySessionEndParams>();

	FLudeoPlayerPendingEndGameplayData* PendingEndGameplayData = new FLudeoPlayerPendingEndGameplayData;
	PendingEndGameplayData->OnEndGameplayDelegate = OnEndGameplayDelegate;
	PendingEndGameplayData->PlayerHandle = PlayerHandle;

	ludeo_GameplaySession_End(PlayerHandle, &EndGameplayParams, PendingEndGameplayData, &FLudeoPlayer::StaticOnEndGameplay);
}

void FLudeoPlayer::OnBeginGameplay(const LudeoGameplaySessionBeginCallbackParams& BeginGameplayCallabckParams) const
{
	if (FLudeoPlayerPendingBeginGameplayData* PendingBeginGameplayData = static_cast<FLudeoPlayerPendingBeginGameplayData*>(BeginGameplayCallabckParams.clientData))
	{
		PendingBeginGameplayData->OnBeginGameplayDelegate.ExecuteIfBound(BeginGameplayCallabckParams.resultCode, *this);
	}
}

void FLudeoPlayer::StaticOnBeginGameplay(const LudeoGameplaySessionBeginCallbackParams* pBeginGameplayCallabckParams)
{
	check(pBeginGameplayCallabckParams != nullptr);
	check(pBeginGameplayCallabckParams->clientData != nullptr);

	if (FLudeoPlayerPendingBeginGameplayData* PendingBeginGameplayData = static_cast<FLudeoPlayerPendingBeginGameplayData*>(pBeginGameplayCallabckParams->clientData))
	{
		if (const FLudeoPlayer* Player = FLudeoPlayer::GetPlayerByPlayerHandle(PendingBeginGameplayData->PlayerHandle))
		{
			Player->OnBeginGameplay(*pBeginGameplayCallabckParams);
		}

		delete PendingBeginGameplayData;
	}
}

void FLudeoPlayer::OnEndGameplay(const LudeoGameplaySessionEndCallbackParams& EndGamepalyCallbackParams) const
{
	if (FLudeoPlayerPendingEndGameplayData* PendingEndGameplayData = static_cast<FLudeoPlayerPendingEndGameplayData*>(EndGamepalyCallbackParams.clientData))
	{
		PendingEndGameplayData->OnEndGameplayDelegate.ExecuteIfBound(EndGamepalyCallbackParams.resultCode, PlayerHandle);
	}
}

void FLudeoPlayer::StaticOnEndGameplay(const LudeoGameplaySessionEndCallbackParams* pEndGamepalyCallbackParams)
{
	check(pEndGamepalyCallbackParams != nullptr);
	check(pEndGamepalyCallbackParams->clientData != nullptr);

	if (FLudeoPlayerPendingEndGameplayData* PendingEndGameplayData = static_cast<FLudeoPlayerPendingEndGameplayData*>(pEndGamepalyCallbackParams->clientData))
	{
		if (const FLudeoPlayer* Player = FLudeoPlayer::GetPlayerByPlayerHandle(PendingEndGameplayData->PlayerHandle))
		{
			Player->OnEndGameplay(*pEndGamepalyCallbackParams);
		}

		delete PendingEndGameplayData;
	}
}
