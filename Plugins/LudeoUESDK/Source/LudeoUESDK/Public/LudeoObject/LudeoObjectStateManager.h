#pragma once

#include "CoreMinimal.h"

#include "Ludeo/Ludeo.h"
#include "LudeoRoom/LudeoRoom.h"
#include "LudeoObject/LudeoObjectStateTypes.h"

// The usage of this class is optional. Game can save states of their game with their custom logic
class LUDEOUESDK_API FLudeoObjectStateManager
{
public:
	typedef TMap<const APlayerState*, FLudeoPlayerHandle> PlayerMapType;

	bool TickUpdateObjectState
	(
		const FLudeoRoom& LudeoRoom,
		const FLudeoObjectStateManager::PlayerMapType& PlayerMap,
		const FLudeoSaveGameSpecification& SaveGameSpecification
	);

	bool RestoreWorld
	(
		const FLudeo& Ludeo,
		const UObject* WorldContextObject,
		const FLudeoSaveGameSpecification& SaveGameSpecification
	);

private:
	static const APlayerState* GetObjectAssociatedPlayerState(const UObject* Object);
	static const FLudeoPlayer* GetObjectAssociatedPlayer(const UObject* Object, const FLudeoObjectStateManager::PlayerMapType& PlayerMap);

	static bool SaveWorld
	(
		const FLudeoRoom& LudeoRoom,
		const FLudeoWritableObject::WritableObjectMapType& ObjectMap,
		const FLudeoObjectStateManager::PlayerMapType& PlayerMap,
		const FLudeoSaveGameSpecification& SaveGameSpecification
	);

	static void CreateSaveWorldObjectMap
	(
		const FLudeoRoom& LudeoRoom,
		const UObject* WorldContextObject,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		FLudeoWritableObject::WritableObjectMapType& OutTrackedLudeoObjectMap
	);

	static FLudeoReadableObject::ReadableObjectMapType CreateRestoreWorldObjectMap
	(
		const FLudeo& Ludeo,
		const UObject* WorldContextObject,
		const FLudeoSaveGameSpecification& SaveGameSpecification
	);

	static TArray<const UObject*> GetObjectsOfClass(const UWorld* World, const TSubclassOf<UObject>& ObjectClass);

	static const FLudeoSaveGameActorData* GetSaveGameActorData
	(
		const TSubclassOf<AActor>& ActorClass,
		const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection
	);

	static const FLudeoSaveGameActorCompomnentData* GetSaveGameActorComponentData
	(
		const TSubclassOf<UActorComponent>& ActorComponentClass,
		const TArray<FLudeoSaveGameActorCompomnentData>& SaveGameActorComponentDataCollection
	);

	static const FLudeoSaveGameActorCompomnentData* GetSaveGameActorComponentData
	(
		const TSubclassOf<AActor>& OuterActorClass,
		const TSubclassOf<UActorComponent>& ActorComponentClass,
		const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection
	);

	static bool IsTargetObjectToBeSaved(const UObject* WorldContextObject, const UObject* Object);

	static TSet<const UObject*> GetObjectToBeSavedSet
	(
		const UObject* WorldContextObject,
		const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection
	);

	static void FindObjectToBeSavedFromProperty
	(
		const UObject* WorldContextObject,
		const UStruct* StructureType,
		const void* MemoryAddress,
		TSet<const UObject*>& ObjectSet,
		TSet<const UObject*>& HasVisitedObjectSet
	);

private:
	FLudeoWritableObject::WritableObjectMapType TrackedLudeoObjectMap;
	FLudeoWritableObject::WritableObjectMapType TrackedScoringLudeoObjectMap;
};
