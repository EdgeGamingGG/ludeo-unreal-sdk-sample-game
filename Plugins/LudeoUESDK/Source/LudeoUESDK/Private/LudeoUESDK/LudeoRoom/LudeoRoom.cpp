#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

#include <Ludeo/Room.h>
#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoCallback/LudeoCallbackManager.h"
#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"

FLudeoRoom::FLudeoRoom(const FLudeoRoomHandle& InRoomHandle, const FLudeoRoomWriterHandle& InRoomWriterHandle) :
	RoomHandle(InRoomHandle),
	RoomWriter(InRoomWriterHandle)
{
	check(RoomHandle != nullptr);

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

	ludeo_Room_AddPlayer
	(
		RoomHandle,
		&InternalAddPlayerParams,
		FLudeoCallbackManager::GetInstance().CreateCallback
		(
			[
				RoomHandle = RoomHandle,
				OnAddPlayerDelegate
			]
			(const LudeoRoomAddPlayerCallbackParams& AddPlayerCallbackParams)
			{
				if(FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(RoomHandle))
				{
					const FLudeoResult Result(AddPlayerCallbackParams.resultCode);

					if (Result.IsSuccessful())
					{
						Room->PlayerCollection.Emplace(AddPlayerCallbackParams.gameplaySession);
					}

					OnAddPlayerDelegate.ExecuteIfBound
					(
						Result,
						RoomHandle,
						AddPlayerCallbackParams.gameplaySession
					);
				}
			}
		),
		[](const LudeoRoomAddPlayerCallbackParams* pAddPlayerCallbackParams)
		{
			FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pAddPlayerCallbackParams);
		}
	);
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

		ludeo_Room_RemovePlayer
		(
			RoomHandle,
			&InternalRemovePlayerParams,
			FLudeoCallbackManager::GetInstance().CreateCallback
			(
				[
					RoomHandle = RoomHandle,
					PlayerHandle = RemovePlayerParameters.PlayerHandle,
					OnRemovePlayerDelegate
				]
				(const LudeoRoomRemovePlayerCallbackParams& RemovePlayerCallbackParams)
				{
					if (FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(RoomHandle))
					{
						const FLudeoResult Result(RemovePlayerCallbackParams.resultCode);

						if (Result.IsSuccessful())
						{
							const int32 PlayerIndex = Room->PlayerCollection.IndexOfByPredicate([&](const FLudeoPlayer& Player)
							{
								return (static_cast<FLudeoPlayerHandle>(Player) == PlayerHandle);
							});

							check(Room->PlayerCollection.IsValidIndex(PlayerIndex));

							if(Room->PlayerCollection.IsValidIndex(PlayerIndex))
							{
								Room->PlayerCollection.RemoveAtSwap(PlayerIndex);
							}
						}

						OnRemovePlayerDelegate.ExecuteIfBound
						(
							Result,
							RoomHandle,
							PlayerHandle
						);
					}
				}
			),
			[](const LudeoRoomRemovePlayerCallbackParams* pRemovePlayerCallbackParams)
			{
				FLudeoCallbackManager::GetInstance().InvokeAndRemoveCallback(pRemovePlayerCallbackParams);
			}
		);
	}
	else
	{
		OnRemovePlayerDelegate.ExecuteIfBound(LudeoResult::NotFound, *this, RemovePlayerParameters.PlayerHandle);
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
