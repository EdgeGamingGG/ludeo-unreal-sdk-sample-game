#pragma once

#include "CoreMinimal.h"

#include "LudeoUESDK/LudeoRoom/LudeoRoomWriterTypes.h"
#include "LudeoUESDK/LudeoObject/LudeoWritableObject.h"

class LUDEOUESDK_API FLudeoRoomWriter
{
public:
	FLudeoRoomWriter(const FLudeoRoomWriterHandle& InRoomWriterHandle);
	~FLudeoRoomWriter();

	FORCEINLINE operator const FLudeoRoomWriterHandle& () const
	{
		return RoomWriterHandle;
	}

public:
	FLudeoResult SetSendSettings(const FLudeoRoomWriterSetSendSettingsParameters& SetSendSettingsParameters) const;

	TOptionalWithLudeoResult<FLudeoWritableObject> CreateObject(const FLudeoRoomWriterCreateObjectParameters& CreateObjectParameters) const;
	FLudeoResult DestroyObject(const FLudeoRoomWriterDestroyObjectParameters& DestroyObjectParameters) const;

	FLudeoResult SendAction(const FLudeoRoomWriterSendActionParameters& SendActionParameters) const;

private:
	FLudeoRoomWriterHandle RoomWriterHandle;
};
