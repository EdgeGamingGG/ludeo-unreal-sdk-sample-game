#pragma once

#include "LudeoUESDK/Ludeo/LudeoTypes.h"
#include "LudeoUESDK/LudeoObject/LudeoReadableObject.h"
#include "LudeoUESDK/LudeoResult.h"

struct FLudeoObjectInformation
{
	FLudeoReadableObject ReadableObject;
	FString ObjectType;
};

class LUDEOUESDK_API FLudeo
{
	friend class FLudeoSession;

public:
	FLudeo(const FLudeoHandle& InLudeoHandle);

	~FLudeo();

	FORCEINLINE operator const FLudeoHandle&() const
	{
		return LudeoHandle;
	}

	static const FLudeo* GetLudeoByLudeoHandle(const FLudeoHandle& LudeoHandle);

public:

	FORCEINLINE const FString& GetLudeoID() const
	{
		return LudeoID;
	}

	FORCEINLINE const FString& GetCreatorPlayerID() const
	{
		return CreatorPlayerID;
	}

	FORCEINLINE const TArray<FLudeoObjectInformation>& GetLudeoObjectInformationCollection() const
	{
		return LudeoObjectInformationCollection;
	}

	FORCEINLINE bool IsValid() const
	{
		return (this == FLudeo::GetLudeoByLudeoHandle(LudeoHandle));
	}

private:
	TArray<FLudeoObjectInformation> FetchLudeoObjectInformationCollection() const;

private:
	FLudeoHandle LudeoHandle;

private:
	FString LudeoID;
	FString CreatorPlayerID;

	TArray<FLudeoObjectInformation> LudeoObjectInformationCollection;
};
