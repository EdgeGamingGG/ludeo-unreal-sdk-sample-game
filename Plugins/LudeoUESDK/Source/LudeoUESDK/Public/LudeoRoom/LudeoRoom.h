#pragma once

#include "LudeoRoom/LudeoRoomTypes.h"

#include "LudeoPlayer/LudeoPlayer.h"
#include "LudeoRoom/LudeoRoomWriter.h"

struct LUDEOUESDK_API FLudeoRoom
{
public:
	FLudeoRoom(const FLudeoRoomHandle& InRoomHandle, const FLudeoRoomWriterHandle& InRoomWriterHandle);
	~FLudeoRoom();

	FORCEINLINE operator FLudeoRoomHandle() const
	{
		return RoomHandle;
	}

	FORCEINLINE const FLudeoRoomWriter& GetRoomWriter() const
	{
		return RoomWriter;
	}

	static FLudeoRoom* GetRoomByRoomHandle(const FLudeoRoomHandle& RoomHandle);

	const FLudeoPlayer* GetPlayerByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle) const;
	const FLudeoPlayer* GetPlayerByPlayerID(const FString& PlayerID) const;

	void AddPlayer
	(
		const FLudeoRoomAddPlayerParameters& AddPlayerParameters,
		const FLudeoRoomOnAddPlayerDelegate& OnAddPlayerDelegate = {}
	) const;

	void RemovePlayer
	(
		const FLudeoRoomRemovePlayerParameters& RemovePlayerParameters,
		const FLudeoRoomOnRemovePlayerDelegate& OnRemovePlayerDelegate = {}
	) const;

	FORCEINLINE const FLudeoRoomInformation& GetRoomInformation() const
	{
		return RoomInformation;
	}

	FORCEINLINE bool IsValid() const
	{
		return (this == FLudeoRoom::GetRoomByRoomHandle(RoomHandle));
	}

private:
	void OnAddPlayer(const LudeoRoomAddPlayerCallbackParams& AddPlayerCallbackParams);
	static void StaticOnAddPlayer(const LudeoRoomAddPlayerCallbackParams* pAddPlayerCallbackParams);

	void OnRemovePlayer(const LudeoRoomRemovePlayerCallbackParams& RemovePlayerCallbackParams);
	static void StaticOnRemovePlayer(const LudeoRoomRemovePlayerCallbackParams* pRemovePlayerCallbackParams);

private:
	FLudeoRoomHandle RoomHandle;
	FLudeoRoomWriter RoomWriter;

	TArray<FLudeoPlayer> PlayerCollection;
	FLudeoRoomInformation RoomInformation;
};
