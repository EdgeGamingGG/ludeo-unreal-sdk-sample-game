#pragma once

#include "LudeoUESDK/LudeoPlayer/LudeoPlayerTypes.h"
#include "LudeoUESDK/LudeoResult.h"

#include "LudeoRoomTypes.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Handle"))
struct FLudeoRoomHandle
{
	GENERATED_BODY()

public:
	FORCEINLINE FLudeoRoomHandle(const LudeoHRoom InRoomHandle = nullptr) :
		RoomHandle(static_cast<uint64>(reinterpret_cast<UPTRINT>(InRoomHandle)))
	{

	}

	FORCEINLINE operator LudeoHRoom() const
	{
		return reinterpret_cast<LudeoHRoom>(static_cast<UPTRINT>(RoomHandle));
	}

private:
	UPROPERTY(Transient)
	uint64 RoomHandle;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoRoomOnAddPlayerDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoRoomHandle&, RoomHandle,
	const FLudeoPlayerHandle&, PlayerHandle
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoRoomOnRemovePlayerDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoRoomHandle&, RoomHandle,
	const FString&, PlayerID
);

DECLARE_DELEGATE_ThreeParams
(
	FLudeoRoomOnAddPlayerDelegate,
	const FLudeoResult&,
	const FLudeoRoomHandle&,
	const FLudeoPlayerHandle&
);

DECLARE_DELEGATE_ThreeParams
(
	FLudeoRoomOnRemovePlayerDelegate,
	const FLudeoResult&,
	const FLudeoRoomHandle&,
	const FString&
);

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Add Player Parameters"))
struct FLudeoRoomAddPlayerParameters
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite)
	FString PlayerID;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Remove Player Parameters"))
struct FLudeoRoomRemovePlayerParameters
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite)
	FString PlayerID;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Get Room Information Parameters"))
struct FLudeoRoomGetRoomInformationParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Remove Information"))
struct FLudeoRoomInformation
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadOnly)
	FString LudeoID;

	UPROPERTY(Transient, BlueprintReadOnly)
	FString RoomID;
};
