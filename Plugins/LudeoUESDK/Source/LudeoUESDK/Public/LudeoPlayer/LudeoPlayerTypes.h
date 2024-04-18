#pragma once

#include "CoreMinimal.h"

#include <Ludeo/GameplaySessionTypes.h>
#include "LudeoResult.h"

#include "LudeoPlayerTypes.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Handle"))
struct FLudeoPlayerHandle
{
	GENERATED_BODY()

public:
	FORCEINLINE FLudeoPlayerHandle(const LudeoHGameplaySession GameplaySessionHandle = nullptr) :
		PlayerHandle(reinterpret_cast<UPTRINT>(GameplaySessionHandle))
	{

	}

	FORCEINLINE operator LudeoHGameplaySession() const
	{
		return reinterpret_cast<LudeoHGameplaySession>(static_cast<UPTRINT>(PlayerHandle));
	}

private:
	UPROPERTY(Transient)
	uint64 PlayerHandle;
};


USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Player Begin Gameplay Parameters"))
struct FLudeoPlayerBeginGameplayParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Player End Gameplay Parameters"))
struct FLudeoPlayerEndGameplayParameters
{
	GENERATED_BODY()

	FLudeoPlayerEndGameplayParameters() :
		bIsAbort(false)
	{

	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bIsAbort;
};

DECLARE_DELEGATE_TwoParams
(
	FLudeoPlayerOnBeginGameplayDelegate,
	const FLudeoResult&,
	const FLudeoPlayerHandle&
);

DECLARE_DELEGATE_TwoParams
(
	FLudeoPlayerOnEndGameplayDelegate,
	const FLudeoResult&,
	const FLudeoPlayerHandle&
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams
(
	FLudeoPlayerOnBeginGameplayDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoPlayerHandle&, PlayerHandle
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams
(
	FLudeoPlayerOnEndGameplayDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoPlayerHandle&, PlayerHandle
);
