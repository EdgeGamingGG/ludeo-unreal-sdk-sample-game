#pragma once

#include <Ludeo/DataReaderTypes.h>

#include "LudeoTypes.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Handle"))
struct LUDEOUESDK_API FLudeoHandle
{
	GENERATED_BODY()
	
	FORCEINLINE FLudeoHandle(const LudeoHDataReader DataReaderHandle = nullptr) :
		LudeoHandle(static_cast<uint64>(reinterpret_cast<UPTRINT>(DataReaderHandle)))
	{

	}

	FORCEINLINE operator LudeoHDataReader() const
	{
		return reinterpret_cast<LudeoHDataReader>(static_cast<UPTRINT>(LudeoHandle));
	}

private:
	UPROPERTY(Transient)
	uint64 LudeoHandle;
};
