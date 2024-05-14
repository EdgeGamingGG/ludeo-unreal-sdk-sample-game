#pragma once

#include "CoreMinimal.h"

#include <Ludeo/DataTypesCommon.h>

#include "LudeoObjectTypes.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Object Handle"))
struct LUDEOUESDK_API FLudeoObjectHandle
{
	GENERATED_BODY()
	
	FORCEINLINE FLudeoObjectHandle(const LudeoObjectId& InObjectID = LUDEO_INVALID_OBJECTID) :
		ObjectID(static_cast<uint64>(InObjectID))
	{
	}

	FORCEINLINE operator LudeoObjectId() const
	{
		return static_cast<LudeoObjectId>(ObjectID);
	}

	FORCEINLINE bool IsValid() const
	{
		return (static_cast<LudeoObjectId>(ObjectID) != LUDEO_INVALID_OBJECTID);
	}

private:
	UPROPERTY(Transient)
	uint64 ObjectID;
};

USTRUCT()
struct FLudeoVector2D : public FVector2D
{
	GENERATED_BODY();
};

USTRUCT()
struct FLudeoVector : public FVector
{
	GENERATED_BODY();
};


USTRUCT()
struct FLudeoVector4 : public FVector4
{
	GENERATED_BODY();
};
USTRUCT()
struct FLudeoRotator : public FRotator
{
	GENERATED_BODY();
};

USTRUCT()
struct FLudeoQuaterion : public FQuat
{
	GENERATED_BODY();
};

USTRUCT()
struct FLudeoTransform : public FTransform
{
	GENERATED_BODY();
};

USTRUCT()
struct FLudeoText
{
	GENERATED_BODY();

	UPROPERTY(Transient)
	FText Text;

	UPROPERTY(Transient, SaveGame)
	FString Namespace;

	UPROPERTY(Transient, SaveGame)
	FName StringTableID;

	UPROPERTY(Transient, SaveGame)
	FString Key;

	UPROPERTY(Transient, SaveGame)
	FString SourceString;
};

struct FLudeoPropertyFilter
{
	bool Match(const FProperty& Proeprty) const
	{
		return Proeprty.HasAllPropertyFlags(MatchingPropertyFlag);
	}

	EPropertyFlags MatchingPropertyFlag = EPropertyFlags::CPF_SaveGame;
};
