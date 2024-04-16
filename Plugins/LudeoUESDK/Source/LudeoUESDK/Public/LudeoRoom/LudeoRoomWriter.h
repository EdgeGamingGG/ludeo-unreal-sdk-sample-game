#pragma once

#include "CoreMinimal.h"

#include "LudeoRoom/LudeoRoomWriterTypes.h"
#include "LudeoObject/LudeoWritableObject.h"

struct LUDEOUESDK_API FLudeoRoomWriter
{
public:
	FLudeoRoomWriter(const FLudeoRoomWriterHandle& InRoomWriterHandle) :
		RoomWriterHandle(InRoomWriterHandle)
	{

	}

public:
	FLudeoResult SetSendSettings(const FLudeoRoomWriterSetSendSettingsParameters& SetSendSettingsParameters) const;

	TOptionalWithLudeoResult<FLudeoWritableObject> CreateObject(const FLudeoRoomWriterCreateObjectParameters& CreateObjectParameters) const;
	FLudeoResult DestroyObject(const FLudeoRoomWriterDestroyObjectParameters& DestroyObjectParameters) const;

	FLudeoResult SendAction(const FLudeoRoomWriterSendActionParameters& SendActionParameters) const;

private:
	FLudeoRoomWriterHandle RoomWriterHandle;
};
