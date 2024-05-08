#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"

#include "LudeoUESDK/LudeoRoom/LudeoRoomTypes.h"

#include "LudeoRoomBlueprintAsyncAction.generated.h"

UCLASS()
class ULudeoRoomAddPlayerAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoRoomPlayerOperationResultDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoRoomPlayerOperationResultDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Room",
			DisplayName = "Ludeo Room Add Player",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoRoomAddPlayerAsyncNode* LudeoRoomAddPlayer
	(
		UObject* WorldContextObject,
		const FLudeoRoomHandle& InRoomHandle,
		const FLudeoRoomAddPlayerParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnAddPlayer(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FLudeoPlayerHandle& InPlayerHandle);

	void OnResultReady(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FLudeoPlayerHandle& InPlayerHandle);

private:
	FLudeoRoomHandle RoomHandle;
	FLudeoRoomAddPlayerParameters Parameters;
};

UCLASS()
class ULudeoRoomRemovePlayerAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoRoomPlayerOperationResultDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoRoomPlayerOperationResultDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Room",
			DisplayName = "Ludeo Room Remove Player",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoRoomRemovePlayerAsyncNode* LudeoRoomRemovePlayer
	(
		UObject* WorldContextObject,
		const FLudeoRoomHandle& InRoomHandle,
		const FLudeoRoomRemovePlayerParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnRemovePlayer(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FLudeoPlayerHandle& InPlayerHandle);

	void OnResultReady(const FLudeoResult& Result, const FLudeoRoomHandle& InRoomHandle, const FLudeoPlayerHandle& InPlayerHandle);

private:
	FLudeoRoomHandle RoomHandle;
	FLudeoRoomRemovePlayerParameters Parameters;
	FString PlayerID;
};