#include "LudeoUESDK/LudeoObject/LudeoObjectStateTypes.h"

const FLudeoSaveGameSubObjectData FLudeoSaveGameSpecification::DefaultSubObjectSaveGameData;

const FLudeoSaveGameActorData* FLudeoSaveGameSpecification::GetSaveGameActorData(const TSubclassOf<AActor>& ActorClass) const
{
	check(ActorClass != nullptr);

	const int32 Index = SaveGameActorDataCollection.FindLastByPredicate([&](const FLudeoSaveGameActorData& SaveGameActorData)
	{
		return SaveGameActorData.ActorFilter.Match(ActorClass);
	});

	if (SaveGameActorDataCollection.IsValidIndex(Index))
	{
		return &SaveGameActorDataCollection[Index];
	}

	return nullptr;
}

const FLudeoSaveGameSubObjectData* FLudeoSaveGameSpecification::GetSaveGameSubObjectData
(
	const TSubclassOf<UObject>& ObjectClass,
	const TArray<FLudeoSaveGameSubObjectData>& SaveGameSubObjectDataCollection
) const
{
	const int32 Index = SaveGameSubObjectDataCollection.FindLastByPredicate
	(
		[&](const FLudeoSaveGameSubObjectData& SaveGameSubObjectData)
		{
			return SaveGameSubObjectData.SubObjectFilter.Match(ObjectClass);
		}
	);

	if (SaveGameSubObjectDataCollection.IsValidIndex(Index))
	{
		return &SaveGameSubObjectDataCollection[Index];
	}

	return nullptr;
}

const FLudeoSaveGameSubObjectData* FLudeoSaveGameSpecification::GetSaveGameSubObjectData
(
	const TSubclassOf<AActor>& OuterActorClass,
	const TSubclassOf<UObject>& ObjectClass
) const
{
	check(ObjectClass != nullptr);

	if (const FLudeoSaveGameActorData* SaveGameActorData = GetSaveGameActorData(OuterActorClass))
	{
		return GetSaveGameSubObjectData(ObjectClass, SaveGameActorData->SaveGameSubObjectDataCollection);
	}

	return nullptr;
}

bool FLudeoSaveGameSpecification::IsSaveGameActor(const AActor* Actor) const
{
	if(Actor != nullptr)
	{
		if (const FLudeoSaveGameActorData* SaveGameActorData = GetSaveGameActorData(Actor->GetClass()))
		{
			return (SaveGameActorData->Strategy != ELudeoSaveGameStrategy::None);
		}
	}

	return false;
}
