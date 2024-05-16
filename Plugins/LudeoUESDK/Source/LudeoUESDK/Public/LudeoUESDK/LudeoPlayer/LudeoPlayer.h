#pragma once

#include "LudeoPlayerTypes.h"

class LUDEOUESDK_API FLudeoPlayer
{
public:
	FLudeoPlayer(const FLudeoPlayerHandle& InPlayerHandle);
	~FLudeoPlayer();

	FORCEINLINE operator FLudeoPlayerHandle() const
	{
		return PlayerHandle;
	}

	static const FLudeoPlayer* GetPlayerByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle);

	const FString& GetPlayerID() const;

	FORCEINLINE bool IsValid() const
	{
		return (this == FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle));
	}

	void BeginGameplay
	(
		const FLudeoPlayerBeginGameplayParameters& BeginGameplayParameters,
		const FLudeoPlayerOnBeginGameplayDelegate& OnBeginGameplayDelegate = {}
	) const;

	void EndGameplay
	(
		const FLudeoPlayerEndGameplayParameters& EndGameplayParameters,
		const FLudeoPlayerOnEndGameplayDelegate& OnEndGameplayDelegate = {}
	) const;

private:
	FLudeoPlayerHandle PlayerHandle;

private:
	FString PlayerID;
};
