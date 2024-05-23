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

	/*
	*	Saving state of objects of the world associated with the WorldContextObject
	*	SearchActorCollection is optional. If it is not set, FActorIteartor is used to search for objects to be saved
	*/
	static bool SaveWorld
	(
		const UObject* WorldContextObject,
		const FLudeoRoom& LudeoRoom,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		const TOptional<TArray<AActor*>>& OptionalSearchActorCollection,
		FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
	);

	// Restore the state the world using the object information in the Ludeo
	static bool RestoreWorld
	(
		const UObject* WorldContextObject,
		const TArray<FLudeoObjectInformation>& ObjectInformationCollection,
		const TArray<TSubclassOf<UObject>>& ObjectClassCollection,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		FLudeoReadableObject::ReadableObjectMapType& CurrentObjectMap
	);

	static const APlayerState* GetObjectAssociatedPlayerState(const UObject* Object);

private:
	static const FLudeoWritableObject* CreateWritableObject
	(
		const FLudeoRoom& LudeoRoom,
		const UObject* Object,
		FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
	);

	static bool DestroyWritableObject
	(
		const FLudeoRoom& LudeoRoom,
		const FLudeoWritableObject& WritableObject,
		FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
	);

	static void UpdateSaveWorldObjectMap
	(
		const UObject* WorldContextObject,
		const FLudeoRoom& LudeoRoom,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		const TOptional<TArray<AActor*>>& OptionalSearchActorCollection,
		FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
	);

	static bool UpdateRestoreWorldObjectMap
	(
		const UObject* WorldContextObject,
		const TArray<FLudeoObjectInformation>& ObjectInformationCollection,
		const TArray<TSubclassOf<UObject>>& ObjectClassCollection,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		FLudeoReadableObject::ReadableObjectMapType& CurrentObjectMap
	);

	static bool IsValidObjectToBeSaved(const UObject* WorldContextObject, const UObject* Object);

	static const TSet<const UObject*>& GetObjectToBeSavedSet
	(
		const UObject* WorldContextObject,
		const TOptional<TArray<AActor*>>& OptionalSearchActorCollection,
		const FLudeoSaveGameSpecification& SaveGameSpecification
	);

	static void FindObjectToBeSavedFromProperty
	(
		const UObject* WorldContextObject,
		const UStruct* StructureType,
		const void* StructureContainer,
		const FLudeoObjectPropertyFilter& ObjectPropertyFilter,
		const FLudeoSaveGameSpecification& SaveGameSpecification,
		TSet<const UObject*>& ObjectSet,
		TSet<const UObject*>& HasVisitedObjectSet
	);

	static TMap<TSubclassOf<AActor>, TArray<AActor*>> GetActorClassMap(const UWorld* World);
};
