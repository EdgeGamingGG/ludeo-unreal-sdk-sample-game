#pragma once

#include "CoreMinimal.h"

#include "LudeoUESDK/Ludeo/Ludeo.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"
#include "LudeoUESDK/LudeoObject/LudeoObjectStateTypes.h"

// The usage of this class is optional. Game can save states of their game with their custom logic
class LUDEOUESDK_API FLudeoObjectStateManager
{
public:
	virtual ~FLudeoObjectStateManager()
	{

	}

	// Saving state of objects of the world associated with the WorldContextObject
	static bool SaveWorld
	(
		const UObject* WorldContextObject,
		const TOptional<FString> LudeoCreatorPlayerID,
		const FLudeoRoom& LudeoRoom,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		FLudeoWritableObject::WritableObjectMapType& ObjectMap
	);

	// Restore the state the world using the object information in the Ludeo
	static bool RestoreWorld
	(
		const UObject* WorldContextObject,
		const TArray<FLudeoObjectInformation>& ObjectInformationCollection,
		const TArray<TSubclassOf<UObject>>& ObjectClassCollection,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		const FLudeoReadableObject::ReadableObjectMapType& InputObjectMap = {}
	);

private:
	static const FLudeoWritableObject* CreateWritableObject
	(
		const FLudeoRoom& LudeoRoom,
		const UObject* Object,
		FLudeoWritableObject::WritableObjectMapType& ObjectMap
	);

	static bool DestroyWritableObject
	(
		const FLudeoRoom& LudeoRoom,
		const FLudeoWritableObject& WritableObject,
		FLudeoWritableObject::WritableObjectMapType& ObjectMap
	);

	static void CreateSaveWorldObjectMap
	(
		const UObject* WorldContextObject,
		const FLudeoRoom& LudeoRoom,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		FLudeoWritableObject::WritableObjectMapType& OutTrackedLudeoObjectMap
	);

	static bool CreateRestoreWorldObjectMap
	(
		const UObject* WorldContextObject,
		const TArray<FLudeoObjectInformation>& ObjectInformationCollection,
		const TArray<TSubclassOf<UObject>>& ObjectClassCollection,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		FLudeoReadableObject::ReadableObjectMapType& ObjectMap
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

	static const APlayerState* GetObjectAssociatedPlayerState(const UObject* Object);

	static TMap<TSubclassOf<AActor>, TArray<const AActor*>> GetActorClassMap(const UWorld* World);

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
};
