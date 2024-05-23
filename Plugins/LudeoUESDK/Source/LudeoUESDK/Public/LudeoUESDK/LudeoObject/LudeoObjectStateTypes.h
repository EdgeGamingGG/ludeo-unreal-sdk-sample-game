#pragma once

#include "LudeoObjectTypes.h"

#include "LudeoObjectStateTypes.generated.h"

UENUM()
enum class ELudeoSaveGameStrategy : uint8
{
	Purge,
	Reconcile,

	None UMETA(Hidden)
};

USTRUCT()
struct FLudeoSaveGameActorFilter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> MatchingActorClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<AActor>> SkipActorClassCollection;

	UPROPERTY(EditDefaultsOnly)
	FLudeoObjectPropertyFilter ActorPropertyFilter;

	bool Match(const TSubclassOf<AActor>& ActorClass) const
	{
		check(MatchingActorClass != nullptr);

		if (ActorClass->IsChildOf(MatchingActorClass))
		{
			return !SkipActorClassCollection.ContainsByPredicate([&](const TSubclassOf<AActor>& SkipActorClass)
			{
				return ActorClass->IsChildOf(SkipActorClass);
			});
		}

		return false;
	}
};

USTRUCT()
struct FLudeoSaveGameSubObjectFilter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UObject> MatchingObjectClass = UObject::StaticClass();

	UPROPERTY(EditDefaultsOnly)
	FLudeoObjectPropertyFilter ObjectPropertyFilter;

	bool Match(const TSubclassOf<UObject>& ObjectClass) const
	{
		check(MatchingObjectClass != nullptr);

		return ObjectClass->IsChildOf(MatchingObjectClass);
	}
};

USTRUCT()
struct FLudeoSaveGameSubObjectData
{
	GENERATED_BODY()

	FLudeoSaveGameSubObjectData() :
		Strategy(ELudeoSaveGameStrategy::Purge)
	{

	}

	UPROPERTY(EditDefaultsOnly)
	FLudeoSaveGameSubObjectFilter SubObjectFilter;

	UPROPERTY(EditDefaultsOnly)
	ELudeoSaveGameStrategy Strategy;
};

USTRUCT()
struct FLudeoSaveGameActorData
{
	GENERATED_BODY()

	FLudeoSaveGameActorData() :
		Strategy(ELudeoSaveGameStrategy::Purge)
	{

	}

	UPROPERTY(EditDefaultsOnly)
	FLudeoSaveGameActorFilter ActorFilter;

	UPROPERTY(EditDefaultsOnly)
	ELudeoSaveGameStrategy Strategy;

	UPROPERTY(EditDefaultsOnly)
	TArray<FLudeoSaveGameSubObjectData> SaveGameSubObjectDataCollection;
};

USTRUCT()
struct LUDEOUESDK_API FLudeoSaveGameSpecification
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FLudeoSaveGameActorData> SaveGameActorDataCollection;

	static const FLudeoSaveGameSubObjectData DefaultSubObjectSaveGameData;

public:
	const FLudeoSaveGameActorData* GetSaveGameActorData(const TSubclassOf<AActor>& ActorClass) const;

	const FLudeoSaveGameSubObjectData* GetSaveGameSubObjectData
	(
		const TSubclassOf<AActor>& OuterActorClass,
		const TSubclassOf<UObject>& ObjectClass
	) const;

	const FLudeoSaveGameSubObjectData* GetSaveGameSubObjectData
	(
		const TSubclassOf<UObject>& ObjectClass,
		const TArray<FLudeoSaveGameSubObjectData>& SaveGameSubObjectDataCollection
	) const;

	bool IsSaveGameActor(const AActor* Actor) const;
};
