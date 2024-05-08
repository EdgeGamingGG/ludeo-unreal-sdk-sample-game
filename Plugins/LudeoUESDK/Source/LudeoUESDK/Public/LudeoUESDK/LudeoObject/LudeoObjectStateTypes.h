#pragma once

#include "CoreMinimal.h"

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

	bool Match(const TSubclassOf<AActor>& ActorClass) const
	{
		check(ActorClass != nullptr);

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
struct FLudeoSaveGameActorCompomnentFilter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UActorComponent> MatchingActorComponentClass;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UActorComponent>> SkipActorComponentClassCollection;

	bool Match(const TSubclassOf<UActorComponent>& ActorComponentClass) const
	{
		check(ActorComponentClass != nullptr);

		if(ActorComponentClass->IsChildOf(MatchingActorComponentClass))
		{
			return !SkipActorComponentClassCollection.ContainsByPredicate([&](const TSubclassOf<UActorComponent>& SkipActorComponentClass)
			{
				return ActorComponentClass->IsChildOf(SkipActorComponentClass);
			});
		}

		return false;
	}
};

USTRUCT()
struct FLudeoSaveGameActorCompomnentData
{
	GENERATED_BODY()

	FLudeoSaveGameActorCompomnentData() :
		Strategy(ELudeoSaveGameStrategy::Reconcile)
	{

	}

	UPROPERTY(EditDefaultsOnly)
	FLudeoSaveGameActorCompomnentFilter ActorCompomnentFilter;

	UPROPERTY(EditDefaultsOnly)
	ELudeoSaveGameStrategy Strategy;

	UPROPERTY(EditDefaultsOnly)
	TArray<FName> AdditionalSaveGamePropertyNameCollection;
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
	TArray<FName> AdditionalSaveGamePropertyNameCollection;

	UPROPERTY(EditDefaultsOnly)
	TArray<FLudeoSaveGameActorCompomnentData> SaveGameActorComponentDataCollection;
};

USTRUCT()
struct FLudeoSaveGameSpecification
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FLudeoSaveGameActorData> SaveGameActorDataCollection;
};
