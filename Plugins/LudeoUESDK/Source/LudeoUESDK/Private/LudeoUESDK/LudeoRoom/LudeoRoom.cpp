#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

#include <Ludeo/Room.h>
#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"

struct FLudeoRoomPendingData
{
	FLudeoRoomHandle RoomHandle;
};

struct FLudeoRoomPendingAddPlayerData : public FLudeoRoomPendingData
{
	FLudeoRoomOnAddPlayerDelegate OnAddPlayerDelegate;
};

struct FPendingRemovePlayerData : public FLudeoRoomPendingData
{
	FLudeoPlayerHandle PlayerHandle;
	FLudeoRoomOnRemovePlayerDelegate OnRemovePlayerDelegate;
};

FLudeoRoom::FLudeoRoom(const FLudeoRoomHandle& InRoomHandle, const FLudeoRoomWriterHandle& InRoomWriterHandle) :
	RoomHandle(InRoomHandle),
	RoomWriter(InRoomWriterHandle)
{
	const LudeoRoomGetInfoParams InternalGetRoomInformationParams = Ludeo::create<LudeoRoomGetInfoParams>();

	LudeoRoomInfo* InternalRoomInformation = nullptr;
		
	const FLudeoResult Result = ludeo_Room_GetInfo(RoomHandle, &InternalGetRoomInformationParams, &InternalRoomInformation);
	check(Result.IsSuccessful());

	if (Result.IsSuccessful())
	{
		check(InternalRoomInformation != nullptr);

		RoomInformation.RoomID = UTF8_TO_TCHAR(InternalRoomInformation->roomId);
		RoomInformation.LudeoID = UTF8_TO_TCHAR(InternalRoomInformation->ludeoId);

		ludeo_RoomInfo_Release(InternalRoomInformation);
	}
}

FLudeoRoom::~FLudeoRoom()
{

}

void FLudeoRoom::AddPlayer
(
	const FLudeoRoomAddPlayerParameters& AddPlayerParameters,
	const FLudeoRoomOnAddPlayerDelegate& OnAddPlayerDelegate
) const
{
	const FTCHARToUTF8 PlayerIDStringConverter
	(
		AddPlayerParameters.PlayerID.GetCharArray().GetData(),
		AddPlayerParameters.PlayerID.GetCharArray().Num()
	);

	LudeoRoomAddPlayerParams InternalAddPlayerParams = Ludeo::create<LudeoRoomAddPlayerParams>();
	InternalAddPlayerParams.playerId = PlayerIDStringConverter.Get();

	FLudeoRoomPendingAddPlayerData* PendingAddPlayerData = new FLudeoRoomPendingAddPlayerData;
	PendingAddPlayerData->OnAddPlayerDelegate = OnAddPlayerDelegate;
	PendingAddPlayerData->RoomHandle = RoomHandle;

	ludeo_Room_AddPlayer(RoomHandle, &InternalAddPlayerParams, PendingAddPlayerData, &FLudeoRoom::StaticOnAddPlayer);
}

void FLudeoRoom::RemovePlayer
(
	const FLudeoRoomRemovePlayerParameters& RemovePlayerParameters,
	const FLudeoRoomOnRemovePlayerDelegate& OnRemovePlayerDelegate
) const
{
	if (const FLudeoPlayer* Player = GetPlayerByPlayerHandle(RemovePlayerParameters.PlayerHandle))
	{
		const FString& PlayerID = Player->GetPlayerID();

		const FTCHARToUTF8 PlayerIDStringConverter
		(
			PlayerID.GetCharArray().GetData(),
			PlayerID.GetCharArray().Num()
		);

		LudeoRoomRemovePlayerParams InternalRemovePlayerParams = Ludeo::create<LudeoRoomRemovePlayerParams>();
		InternalRemovePlayerParams.playerId = PlayerIDStringConverter.Get();

		FPendingRemovePlayerData* PendingRemovePlayerData = new FPendingRemovePlayerData;
		PendingRemovePlayerData->PlayerHandle = RemovePlayerParameters.PlayerHandle;
		PendingRemovePlayerData->OnRemovePlayerDelegate = OnRemovePlayerDelegate;
		PendingRemovePlayerData->RoomHandle = RoomHandle;

		ludeo_Room_RemovePlayer(RoomHandle, &InternalRemovePlayerParams, PendingRemovePlayerData, &FLudeoRoom::StaticOnRemovePlayer);
	}
	else
	{
		OnRemovePlayerDelegate.ExecuteIfBound(LudeoResult::NotFound, *this, RemovePlayerParameters.PlayerHandle);
	}
}

void FLudeoRoom::OnAddPlayer(const LudeoRoomAddPlayerCallbackParams& AddPlayerCallbackParams)
{
	if (FLudeoRoomPendingAddPlayerData* PendingAddPlayerData = static_cast<FLudeoRoomPendingAddPlayerData*>(AddPlayerCallbackParams.clientData))
	{
		const FLudeoResult Result(AddPlayerCallbackParams.resultCode);

		if (Result.IsSuccessful())
		{
			PlayerCollection.Emplace(AddPlayerCallbackParams.gameplaySession);
		}

		PendingAddPlayerData->OnAddPlayerDelegate.ExecuteIfBound
		(
			Result,
			RoomHandle,
			AddPlayerCallbackParams.gameplaySession
		);
	}
}

void FLudeoRoom::StaticOnAddPlayer(const LudeoRoomAddPlayerCallbackParams* pAddPlayerCallbackParams)
{
	check(pAddPlayerCallbackParams != nullptr);
	check(pAddPlayerCallbackParams->clientData != nullptr);

	if (FLudeoRoomPendingAddPlayerData* PendingAddPlayerData = static_cast<FLudeoRoomPendingAddPlayerData*>(pAddPlayerCallbackParams->clientData))
	{
		if (FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(PendingAddPlayerData->RoomHandle))
		{
			LudeoRoom->OnAddPlayer(*pAddPlayerCallbackParams);
		}

		delete PendingAddPlayerData;
	}
}

void FLudeoRoom::OnRemovePlayer(const LudeoRoomRemovePlayerCallbackParams& RemovePlayerCallbackParams)
{
	if (FPendingRemovePlayerData* PendingRemovePlayerData = static_cast<FPendingRemovePlayerData*>(RemovePlayerCallbackParams.clientData))
	{
		const FLudeoResult Result(RemovePlayerCallbackParams.resultCode);

		if (Result.IsSuccessful())
		{
			const int32 PlayerIndex = PlayerCollection.IndexOfByPredicate([&](const FLudeoPlayer& Player)
			{
				return (static_cast<FLudeoPlayerHandle>(Player) == PendingRemovePlayerData->PlayerHandle);
			});

			check(PlayerCollection.IsValidIndex(PlayerIndex));

			PlayerCollection.RemoveAtSwap(PlayerIndex);
		}

		PendingRemovePlayerData->OnRemovePlayerDelegate.ExecuteIfBound
		(
			Result,
			RoomHandle,
			PendingRemovePlayerData->PlayerHandle
		);
	}
}

void FLudeoRoom::StaticOnRemovePlayer(const LudeoRoomRemovePlayerCallbackParams* pRemovePlayerCallbackParams)
{
	check(pRemovePlayerCallbackParams != nullptr);
	check(pRemovePlayerCallbackParams->clientData != nullptr);

	if (FPendingRemovePlayerData* PendingRemovePlayerData = static_cast<FPendingRemovePlayerData*>(pRemovePlayerCallbackParams->clientData))
	{
		if (FLudeoRoom* LudeoRoom = FLudeoRoom::GetRoomByRoomHandle(PendingRemovePlayerData->RoomHandle))
		{
			LudeoRoom->OnRemovePlayer(*pRemovePlayerCallbackParams);
		}

		delete PendingRemovePlayerData;
	}
}

const FLudeoPlayer* FLudeoRoom::GetPlayerByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle) const
{
	if (PlayerHandle != nullptr)
	{
		const int32 PlayerIndex = PlayerCollection.IndexOfByPredicate([&](const FLudeoPlayer& Player)
		{
			return (static_cast<FLudeoPlayerHandle>(Player) == PlayerHandle);
		});

		if (PlayerCollection.IsValidIndex(PlayerIndex))
		{
			return &PlayerCollection[PlayerIndex];
		}
	}

	return nullptr;
}

const FLudeoPlayer* FLudeoRoom::GetPlayerByPlayerID(const FString& PlayerID) const
{
	const int32 PlayerIndex = PlayerCollection.IndexOfByPredicate([&](const FLudeoPlayer& Player)
	{
		return (Player.GetPlayerID() == PlayerID);
	});

	if (PlayerCollection.IsValidIndex(PlayerIndex))
	{
		return &PlayerCollection[PlayerIndex];
	}

	return nullptr;
}

FLudeoRoom* FLudeoRoom::GetRoomByRoomHandle(const FLudeoRoomHandle& RoomHandle)
{
	if (FLudeoSession* Session = FLudeoSessionManager::StaticGetSessionByRoomHandle(RoomHandle))
	{
		return Session->GetRoomByRoomHandle(RoomHandle);
	}

	return nullptr;
}
