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
	GENERATED_BODY()
};

USTRUCT()
struct FLudeoVector : public FVector
{
	GENERATED_BODY()
};


USTRUCT()
struct FLudeoVector4 : public FVector4
{
	GENERATED_BODY()
};
USTRUCT()
struct FLudeoRotator : public FRotator
{
	GENERATED_BODY()
};

USTRUCT()
struct FLudeoQuaterion : public FQuat
{
	GENERATED_BODY()
};

USTRUCT()
struct FLudeoTransform : public FTransform
{
	GENERATED_BODY()
};

USTRUCT()
struct FLudeoText
{
	GENERATED_BODY()

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

UENUM(meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ELudeoObjectPropertyFlags : uint64
{
	None				= EPropertyFlags::CPF_None,
	SaveGame			= EPropertyFlags::CPF_SaveGame
};

USTRUCT()
struct FLudeoObjectPropertyClassFilter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UObject>> MatchingPropertyClassCollection;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UObject>> MismatchingPropertyClassCollection;

	bool Match(const TSubclassOf<UObject>& PropertyClass) const
	{
		return
		(
			MatchingPropertyClassCollection.ContainsByPredicate([&](const TSubclassOf<UObject>& MatchingPropertyClass)
			{
				return PropertyClass->IsChildOf(MatchingPropertyClass);
			})
			&&
			!MismatchingPropertyClassCollection.ContainsByPredicate([&](const TSubclassOf<UObject>& MismatchingPropertyClass)
			{
				return PropertyClass->IsChildOf(MismatchingPropertyClass);
			})
		);
	}
};

USTRUCT()
struct FLudeoObjectPropertyNameFilter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> PropertyNameCollection;

	bool Match(const FName& PropertyName) const
	{
		return PropertyNameCollection.Contains(PropertyName);
	}
};

USTRUCT()
struct FLudeoObjectPropertyFilter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (Bitmask, BitmaskEnum = "ELudeoObjectPropertyFlags"))
	uint64 MatchingPropertyFlags = static_cast<uint64>(ELudeoObjectPropertyFlags::SaveGame);

	UPROPERTY(EditDefaultsOnly)
	FLudeoObjectPropertyNameFilter MatchingPropertyNameFilter;

	UPROPERTY(EditDefaultsOnly)
	FLudeoObjectPropertyClassFilter MatchingPropertyClassFilter;

	bool Match(const FProperty& Proeprty) const
	{
		if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(&Proeprty))
		{
			if (MatchingPropertyClassFilter.Match(ObjectProperty->PropertyClass))
			{
				return true;
			}
		}

		return
		(
			Proeprty.HasAnyPropertyFlags(MatchingPropertyFlags)		||
			MatchingPropertyNameFilter.Match(Proeprty.GetFName())
		);
	}
};
