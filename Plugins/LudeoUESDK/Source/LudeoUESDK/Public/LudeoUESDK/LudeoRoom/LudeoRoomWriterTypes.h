#pragma once

#include "CoreMinimal.h"

#include <Ludeo/DataWriterTypes.h>

#include "LudeoUESDK/LudeoPlayer/LudeoPlayerTypes.h"
#include "LudeoUESDK/LudeoObject/LudeoObjectTypes.h"
#include "LudeoUESDK/LudeoResult.h"

#include "LudeoRoomWriterTypes.generated.h"

static_assert(sizeof(uint32) == sizeof(LudeoObjectId), "Ludeo Object ID size mismatch");

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Writer Handle"))
struct LUDEOUESDK_API FLudeoRoomWriterHandle
{
	GENERATED_BODY()
	
	FORCEINLINE FLudeoRoomWriterHandle(const LudeoHDataWriter DataWriterHandle = nullptr) :
		RoomWriterHandle(static_cast<uint64>(reinterpret_cast<UPTRINT>(DataWriterHandle)))
	{
	}

	FORCEINLINE operator LudeoHDataWriter() const
	{
		return reinterpret_cast<LudeoHDataWriter>(static_cast<UPTRINT>(RoomWriterHandle));
	}

private:
	UPROPERTY(Transient)
	uint64 RoomWriterHandle;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Writer Set Send Settings Parameters"))
struct FLudeoRoomWriterSetSendSettingsParameters
{
	GENERATED_BODY()

	FLudeoRoomWriterSetSendSettingsParameters() :
		SendInteralMS(0)
	{

	}

	UPROPERTY(Transient, BlueprintReadWrite, meta = (ClampMin = "0", ClampMax = "4294967295", UIMin = "0", UIMax = "4294967295"))
	int64 SendInteralMS;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Writer Create Object Parameters"))
struct FLudeoRoomWriterCreateObjectParameters
{
	GENERATED_BODY()

	FLudeoRoomWriterCreateObjectParameters() :
		Object(nullptr)
	{

	}

	UPROPERTY(Transient, BlueprintReadWrite)
	FLudeoObjectHandle ObjectHandle;

	UPROPERTY(Transient, BlueprintReadWrite)
	const UObject* Object;

	// If ObjectType is empty, Object->GetClass()->GetName() will be used as object type
	UPROPERTY(Transient, BlueprintReadWrite)
	FString ObjectType;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Writer Destroy Object Parameters"))
struct FLudeoRoomWriterDestroyObjectParameters
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite)
	FLudeoObjectHandle ObjectHandle;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Room Writer Send Action Parameters"))
struct FLudeoRoomWriterSendActionParameters
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite)
	FString PlayerID;

	UPROPERTY(Transient, BlueprintReadWrite)
	FString ActionName;
};
