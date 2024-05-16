#include "LudeoUESDK/LudeoPlayer/LudeoPlayer.h"

#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoCallback/LudeoCallbackManager.h"
#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

FLudeoPlayer::FLudeoPlayer(const FLudeoPlayerHandle& InPlayerHandle) :
	PlayerHandle(InPlayerHandle)
{
	check(PlayerHandle != nullptr);
	
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

	ludeo_GameplaySession_Begin
	(
		PlayerHandle,
		&BeginGameplayParams,
		FLudeoCallbackManager::GetInstance().CreateCallback
		(
			[
				PlayerHandle = PlayerHandle,
				OnBeginGameplayDelegate
			]
			(const LudeoGameplaySessionBeginCallbackParams& BeginGameplayCallabckParams)
			{
				if(const FLudeoPlayer* Player = FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle))
				{
					OnBeginGameplayDelegate.ExecuteIfBound(BeginGameplayCallabckParams.resultCode, *Player);
				}
			}
		),
		[](const LudeoGameplaySessionBeginCallbackParams* pBeginGameplayCallabckParams)
		{
			FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pBeginGameplayCallabckParams);
		}
	);
}

void FLudeoPlayer::EndGameplay
(
	const FLudeoPlayerEndGameplayParameters& EndGameplayParameters,
	const FLudeoPlayerOnEndGameplayDelegate& OnEndGameplayDelegate
) const
{
	LudeoGameplaySessionEndParams EndGameplayParams = Ludeo::create<LudeoGameplaySessionEndParams>();
	EndGameplayParams.isAbort = (EndGameplayParameters.bIsAbort ? LUDEO_TRUE : LUDEO_FALSE);

	ludeo_GameplaySession_End
	(
		PlayerHandle,
		&EndGameplayParams,
		FLudeoCallbackManager::GetInstance().CreateCallback
		(
			[
				PlayerHandle = PlayerHandle,
				OnEndGameplayDelegate
			]
			(const LudeoGameplaySessionEndCallbackParams& EndGamepalyCallbackParams)
			{
				if (const FLudeoPlayer* Player = FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle))
				{
					OnEndGameplayDelegate.ExecuteIfBound(EndGamepalyCallbackParams.resultCode, *Player);
				}
			}
		),
		[](const LudeoGameplaySessionEndCallbackParams* pEndGamepalyCallbackParams)
		{
			FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pEndGamepalyCallbackParams);
		}
	);
}
