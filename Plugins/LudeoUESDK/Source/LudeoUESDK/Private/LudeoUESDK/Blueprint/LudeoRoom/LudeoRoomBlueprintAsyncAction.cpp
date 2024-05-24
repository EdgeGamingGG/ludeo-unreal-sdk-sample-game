#include "LudeoUESDK/Blueprint/LudeoRoom/LudeoRoomBlueprintAsyncAction.h"

#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

ULudeoRoomAddPlayerAsyncNode* ULudeoRoomAddPlayerAsyncNode::AddPlayer
(
	UObject* WorldContextObject,
	const FLudeoRoomHandle& InRoomHandle,
	const FLudeoRoomAddPlayerParameters& InParameters
)
{
	ULudeoRoomAddPlayerAsyncNode* AsyncNode = NewObject<ULudeoRoomAddPlayerAsyncNode>(WorldContextObject);
	AsyncNode->RoomHandle = InRoomHandle;
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoRoomAddPlayerAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Closing room..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(RoomHandle))
	{
		Room->AddPlayer
		(
			Parameters,
			FLudeoRoomOnAddPlayerDelegate::CreateUObject(this, &ULudeoRoomAddPlayerAsyncNode::OnAddPlayer)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, RoomHandle, nullptr);
	}
}

void ULudeoRoomAddPlayerAsyncNode::OnAddPlayer(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FLudeoPlayerHandle& InPlayerHandle)
{
	OnResultReady(Result, InRoomHandle, InPlayerHandle);
}

void ULudeoRoomAddPlayerAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FLudeoPlayerHandle& InPlayerHandle)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Player %s added successfully"),
				*Parameters.PlayerID
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnSuccessDelegate.Broadcast(Result, InRoomHandle, InPlayerHandle);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to add player %s, reason: %s"),
				*Parameters.PlayerID,
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InRoomHandle, InPlayerHandle);
	}
}


ULudeoRoomRemovePlayerAsyncNode* ULudeoRoomRemovePlayerAsyncNode::RemovePlayer
(
	UObject* WorldContextObject,
	const FLudeoRoomHandle& InRoomHandle,
	const FLudeoRoomRemovePlayerParameters& InParameters
)
{
	ULudeoRoomRemovePlayerAsyncNode* AsyncNode = NewObject<ULudeoRoomRemovePlayerAsyncNode>(WorldContextObject);
	AsyncNode->RoomHandle = InRoomHandle;
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoRoomRemovePlayerAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Closing room..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (FLudeoRoom* Room = FLudeoRoom::GetRoomByRoomHandle(RoomHandle))
	{
		Room->RemovePlayer
		(
			Parameters,
			FLudeoRoomOnRemovePlayerDelegate::CreateUObject(this, &ULudeoRoomRemovePlayerAsyncNode::OnRemovePlayer)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, RoomHandle, Parameters.PlayerID);
	}
}

void ULudeoRoomRemovePlayerAsyncNode::OnRemovePlayer(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FString& PlayerID)
{
	OnResultReady(Result, InRoomHandle, PlayerID);
}

void ULudeoRoomRemovePlayerAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FString& PlayerID)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Player %s removed successfully"),
				*PlayerID
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnSuccessDelegate.Broadcast(Result, InRoomHandle, PlayerID);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to remove player %s, reason: %s"),
				*PlayerID,
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InRoomHandle, PlayerID);
	}
}
