#pragma once

#include "LudeoTypes.h"
#include "LudeoObject/LudeoReadableObject.h"
#include "LudeoResult.h"

struct FLudeoObjectInformation
{
	FLudeoReadableObject ReadableObject;
	TSubclassOf<UObject> ObjectClass;
	FString ObjectType;
};

struct LUDEOUESDK_API FLudeo
{
	friend struct FLudeoSession;

public:
	FLudeo(const FLudeoHandle& InLudeoHandle);

	~FLudeo();

	FORCEINLINE operator FLudeoHandle() const
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

	FString LudeoID;
	FString CreatorPlayerID;
	TArray<FLudeoObjectInformation> LudeoObjectInformationCollection;
};
