#include "LudeoUESDK/LudeoObject/LudeoObjectStateManager.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"

#include "LudeoUESDK/LudeoScopedGuard.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoom.h"

const FLudeoWritableObject* FLudeoObjectStateManager::CreateWritableObject
(
	const FLudeoRoom& LudeoRoom,
	const UObject* Object,
	FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
)
{
	FLudeoRoomWriterCreateObjectParameters CreateObjectParameters;
	CreateObjectParameters.Object = Object;

	const TOptionalWithLudeoResult<FLudeoWritableObject> Result = LudeoRoom.GetRoomWriter().CreateObject(CreateObjectParameters);

	if(Result.IsSuccessful())
	{
		const FLudeoWritableObject& WritableObject = CurrentObjectMap.Emplace(Object, Result.GetValue());

		return &WritableObject;
	}

	return nullptr;
}

bool FLudeoObjectStateManager::DestroyWritableObject
(
	const FLudeoRoom& LudeoRoom,
	const FLudeoWritableObject& WritableObject,
	FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
)
{
	FLudeoRoomWriterDestroyObjectParameters DestroyObjectParameters;
	DestroyObjectParameters.ObjectHandle = WritableObject;

	const FLudeoResult Result = LudeoRoom.GetRoomWriter().DestroyObject(DestroyObjectParameters);
	
	if (Result.IsSuccessful())
	{
		CurrentObjectMap.Remove(WritableObject.GetObject());
	}

	return Result.IsSuccessful();
}

void FLudeoObjectStateManager::UpdateSaveWorldObjectMap
(
	const UObject* WorldContextObject,
	const FLudeoRoom& LudeoRoom,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	const TOptional<TArray<AActor*>>& OptionalSearchActorCollection,
	FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	// Destroy objects
	for (FLudeoWritableObject::WritableObjectMapType::TIterator Itr = CurrentObjectMap.CreateIterator(); Itr; ++Itr)
	{
		const FLudeoWritableObject& WritableObject = Itr->Get<1>();

		if (WritableObject.GetObject() == nullptr)
		{
			FLudeoRoomWriterDestroyObjectParameters DestroyObjectParameters;
			DestroyObjectParameters.ObjectHandle = WritableObject;

			const FLudeoResult Result = LudeoRoom.GetRoomWriter().DestroyObject(DestroyObjectParameters);
			check(Result.IsSuccessful());

			Itr.RemoveCurrent();
		}
	}

	// Create Object
	{
		const TSet<const UObject*>& ThisFrameObjectSet = FLudeoObjectStateManager::GetObjectToBeSavedSet
		(
			World,
			OptionalSearchActorCollection,
			SaveGameSpecification
		);

		if (ThisFrameObjectSet.Num() > 0)
		{
			for (const UObject* Object : ThisFrameObjectSet)
			{
				if (!CurrentObjectMap.Contains(Object))
				{
					const FLudeoWritableObject* WritableObject = CreateWritableObject(LudeoRoom, Object, CurrentObjectMap);
					check(WritableObject != nullptr);
				}
			}
		}
	}
}

bool FLudeoObjectStateManager::UpdateRestoreWorldObjectMap
(
	const UObject* WorldContextObject,
	const TArray<FLudeoObjectInformation>& ObjectInformationCollection,
	const TArray<TSubclassOf<UObject>>& ObjectClassCollection,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	FLudeoReadableObject::ReadableObjectMapType& CurrentObjectMap
)
{
	check(ObjectInformationCollection.Num() == ObjectClassCollection.Num());

	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection = SaveGameSpecification.SaveGameActorDataCollection;

	if(SaveGameActorDataCollection.Num() > 0)
	{
		const auto GetActorClassSaveGameStrategry = [&](const TSubclassOf<AActor>& ActorClass)
		{
			if(ActorClass != nullptr)
			{
				if (const FLudeoSaveGameActorData* SaveGameActorData = SaveGameSpecification.GetSaveGameActorData(ActorClass))
				{
					return SaveGameActorData->Strategy;
				}
			}

			return ELudeoSaveGameStrategy::None;
		};

		const auto GetOuterObjectLudeoObjectHandle = [](const FLudeoReadableObject& ReadableObject)
		{
			FLudeoObjectHandle LudeoObjectHandle;
			const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObject);
			const bool bHasReadSuccessfully = ReadableObject.ReadData(TEXT("OuterPrivate"), LudeoObjectHandle);
			check(bHasReadSuccessfully);

			check(LudeoObjectHandle != LUDEO_INVALID_OBJECTID);

			return LudeoObjectHandle;
		};

		// Destroy all actors that needs to be purged first
		for (TActorIterator<AActor> Itr(World); Itr; ++Itr)
		{
			if (GetActorClassSaveGameStrategry(Itr->GetClass()) == ELudeoSaveGameStrategy::Purge)
			{
				World->DestroyActor(*Itr);
			}
		}

		const FLudeoReadableObject::ReadableObjectMapType InitialObjectMap = CurrentObjectMap;

		TMap<const UObject*, FLudeoReadableObject> InvertedCurrentObjectMap;
		{
			for (FLudeoReadableObject::ReadableObjectMapType::TConstIterator Itr = CurrentObjectMap.CreateConstIterator(); Itr; ++Itr)
			{
				InvertedCurrentObjectMap.Emplace(Itr->Value, Itr->Key);
			}
		}

		{
			// Get an actor class map for reconciling
			const TMap<TSubclassOf<AActor>, TArray<AActor*>> ActorClassMap = FLudeoObjectStateManager::GetActorClassMap(World);

			// Iterate through the object information collection
			for(int32 i = 0; i < ObjectInformationCollection.Num(); ++i)
			{
				const FLudeoReadableObject& ReadableObject = ObjectInformationCollection[i].ReadableObject;
				const TSubclassOf<UObject>& ObjectClass = ObjectClassCollection[i];

				// Skip the data that has already has a mapping
				if(!InitialObjectMap.Contains(ReadableObject))
				{
					AActor* Actor = [&]()
					{
						// Create mapping for actor object first
						if(ObjectClass->IsChildOf(AActor::StaticClass()))
						{
							if (const FLudeoSaveGameActorData* SaveGameActorData = SaveGameSpecification.GetSaveGameActorData(ObjectClass.Get()))
							{
								if(SaveGameActorData->Strategy == ELudeoSaveGameStrategy::Reconcile)
								{
									const TArray<AActor*>* pObjectCollection = ActorClassMap.Find(ObjectClass.Get());
									check(pObjectCollection != nullptr);

									if(pObjectCollection != nullptr)
									{
										const TArray<AActor*> ActorCollection = pObjectCollection->FilterByPredicate([&](const AActor* Actor)
										{
											return !InvertedCurrentObjectMap.Contains(Actor);
										});
										check(ActorCollection.Num() == 1);

										if(ActorCollection.Num() == 1)
										{
											return Cast<AActor>(ActorCollection.Last());
										}
									}
								}
								else if(SaveGameActorData->Strategy == ELudeoSaveGameStrategy::Purge)
								{
									FActorSpawnParameters ActorSpawnParameters;
									ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

									AActor* Actor = World->SpawnActor<AActor>(ObjectClass, ActorSpawnParameters);
									check(Actor != nullptr);

									// Destroy actor component objects that has to be purged
									for (TFieldIterator<FObjectProperty> PropertyIterator(Actor->GetClass()); PropertyIterator; ++PropertyIterator)
									{
										FObjectProperty* ObjectProperty = *PropertyIterator;
										check(ObjectProperty != nullptr);

										if(SaveGameActorData->ActorFilter.ActorPropertyFilter.Match(*ObjectProperty))
										{
											if (UObject* Object = ObjectProperty->GetObjectPropertyValue_InContainer(Actor))
											{
												const ELudeoSaveGameStrategy SaveGameStrategy = [&]()
												{
													const FLudeoSaveGameSubObjectData* SaveGameSubObjectData = SaveGameSpecification.GetSaveGameSubObjectData
													(
														Actor->GetClass(),
														Object->GetClass()
													);

													if (SaveGameSubObjectData != nullptr)
													{
														return SaveGameSubObjectData->Strategy;
													}

													return SaveGameSpecification.DefaultSubObjectSaveGameData.Strategy;
												}();
										
												if (SaveGameStrategy == ELudeoSaveGameStrategy::Purge)
												{
													if(UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
													{
														ActorComponent->DestroyComponent(true);
													}

													ObjectProperty->SetObjectPropertyValue_InContainer(Actor, nullptr);
												}
											}
										}
									}

									return Actor;
								}
							}

							check(false);
						}

						return static_cast<AActor*>(nullptr);
					}();

					if(Actor != nullptr)
					{
						check(Actor->GetClass() == ObjectClass);

						CurrentObjectMap.Emplace(ReadableObject, Actor);
						InvertedCurrentObjectMap.Emplace(Actor, ReadableObject);
					}
				}
			}
		}

		for (int32 i = 0; i < ObjectInformationCollection.Num(); ++i)
		{
			const FLudeoReadableObject& ReadableObject = ObjectInformationCollection[i].ReadableObject;
			const TSubclassOf<UObject>& ObjectClass = ObjectClassCollection[i];
	
			// Skip the data that has already has a mapping
			if (!InitialObjectMap.Contains(ReadableObject))
			{
				UObject* Object = [&]()
				{
					// Create mapping for the non-actor object
					if(!ObjectClass->IsChildOf(AActor::StaticClass()))
					{
						const FLudeoObjectHandle OuterLudeoObjectHandle = GetOuterObjectLudeoObjectHandle(ReadableObject);

						if(const UObject* const* pOuterObject = CurrentObjectMap.FindByHash(GetTypeHash(OuterLudeoObjectHandle), OuterLudeoObjectHandle))
						{
							const FLudeoSaveGameSubObjectData& SaveGameSubObjectData = [&]()
							{
								const TSubclassOf<AActor> OuterActorClass = [&]()
								{
									if (const AActor* OuterActor = Cast<const AActor>(*pOuterObject))
									{
										return OuterActor->GetClass();
									}
									else if (const AActor* TypedOuterActor = (*pOuterObject)->GetTypedOuter<AActor>())
									{
										return TypedOuterActor->GetClass();
									}

									return static_cast<UClass*>(nullptr);
								}();

								if(OuterActorClass != nullptr)
								{
									const FLudeoSaveGameSubObjectData* pSaveGameSubObjectData = SaveGameSpecification.GetSaveGameSubObjectData
									(
										OuterActorClass,
										ObjectClass.Get()
									);

									if (pSaveGameSubObjectData != nullptr)
									{
										return *pSaveGameSubObjectData;
									}
								}						

								return SaveGameSpecification.DefaultSubObjectSaveGameData;
							}();

							if (SaveGameSubObjectData.Strategy == ELudeoSaveGameStrategy::Purge)
							{
								return NewObject<UObject>(const_cast<UObject*>(*pOuterObject), ObjectClass);
							}
							else if (SaveGameSubObjectData.Strategy == ELudeoSaveGameStrategy::Reconcile)
							{
								for (
									const UObject* CurrentOuterObject = *pOuterObject;
									CurrentOuterObject != nullptr && !CurrentOuterObject->IsA(ULevel::StaticClass());
									CurrentOuterObject = CurrentOuterObject->GetOuter()
								)
								{
									const FLudeoReadableObject& OuterActorReadableObject = InvertedCurrentObjectMap.FindChecked(CurrentOuterObject);

									const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(OuterActorReadableObject);

									for (TFieldIterator<FObjectProperty> PropertyIterator(CurrentOuterObject->GetClass()); PropertyIterator; ++PropertyIterator)
									{
										FObjectProperty* ObjectProperty = *PropertyIterator;
										check(ObjectProperty != nullptr);

										FLudeoObjectHandle LudeoObjectHandle;

										if (OuterActorReadableObject.ReadData(*ObjectProperty->GetName(), LudeoObjectHandle))
										{
											if (LudeoObjectHandle == static_cast<FLudeoObjectHandle>(ReadableObject))
											{
												if(UObject* Object = ObjectProperty->GetObjectPropertyValue_InContainer(CurrentOuterObject))
												{
													return Object;
												}
											}
										}
									}
								}
								
								UObject* TargetObject = nullptr;
								{
									for (FUnsafeObjectIterator Itr(ObjectClass); Itr; ++Itr)
									{
										if (World == Itr->GetWorld())
										{
											if (TargetObject == nullptr)
											{
												TargetObject = *Itr;
											}
											else
											{
												check(false);
											}
										}
									}
								}

								check(TargetObject != nullptr);
								return TargetObject;
							}
						
							check(false);
						}
						else
						{
							return NewObject<UObject>(GetTransientPackage(), ObjectClass);
						}
					}

					return static_cast<UObject*>(nullptr);
				}();

				if(Object != nullptr)
				{
					check(Object->GetClass() == ObjectClass);

					CurrentObjectMap.Emplace(ReadableObject, Object);
					InvertedCurrentObjectMap.Emplace(Object, ReadableObject);
				}
			}
		}

		// Restore outer of object created
		for (const TPair<const UObject*, FLudeoReadableObject>& Pair : InvertedCurrentObjectMap)
		{
			UObject* const& Object = const_cast<UObject*>(Pair.Get<0>());

			if(Object->GetOuter() == GetTransientPackage())
			{
				const FLudeoObjectHandle OuterLudeoObjectHandle = GetOuterObjectLudeoObjectHandle(Pair.Get<1>());
					
				const UObject* const* OuterObject = CurrentObjectMap.FindByHash(GetTypeHash(OuterLudeoObjectHandle), OuterLudeoObjectHandle);
				check(OuterObject != nullptr);

				Object->Rename(nullptr, const_cast<UObject*>(*OuterObject));
			}
		}
	}

	return true;
}

bool FLudeoObjectStateManager::IsValidObjectToBeSaved(const UObject* WorldContextObject, const UObject* Object)
{
	check(WorldContextObject != nullptr);

	return
	(
		(Object != nullptr) &&
		(Object->GetWorld() == WorldContextObject->GetWorld()) &&
		!Object->IsPendingKillOrUnreachable() &&
		!Object->HasAnyFlags(RF_ClassDefaultObject) &&
		(Object->GetOutermost() != GetTransientPackage())
	);
}

void FLudeoObjectStateManager::FindObjectToBeSavedFromProperty
(
	const UObject* WorldContextObject,
	const UStruct* StructureType,
	const void* StructureContainer,
	const FLudeoObjectPropertyFilter& ObjectPropertyFilter,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	TSet<const UObject*>& ObjectSet,
	TSet<const UObject*>& HasVisitedObjectSet
)
{
	const auto HandleNonContainerProperty = [&](const FProperty& Property, const void* PropertyContainer)
	{
		if (const FStructProperty* StructProperty = CastField<FStructProperty>(&Property))
		{
			FLudeoObjectPropertyFilter NewObjectPropertyFilter;
			NewObjectPropertyFilter.MatchingPropertyFlags = ObjectPropertyFilter.MatchingPropertyFlags;

			FLudeoObjectStateManager::FindObjectToBeSavedFromProperty
			(
				WorldContextObject,
				StructProperty->Struct,
				StructProperty->ContainerPtrToValuePtr<void>(PropertyContainer),
				NewObjectPropertyFilter,
				SaveGameSpecification,
				ObjectSet,
				HasVisitedObjectSet
			);
		}
		else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(&Property))
		{
			const UObject* Object = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

			if (Object != nullptr)
			{
				if (!HasVisitedObjectSet.Contains(Object))
				{
					HasVisitedObjectSet.Add(Object);

					if (AActor* OuterActor = Object->GetTypedOuter<AActor>())
					{
						if (FLudeoObjectStateManager::IsValidObjectToBeSaved(WorldContextObject, Object))
						{
							ObjectSet.Add(Object);
						}

						if (const FLudeoSaveGameSubObjectData* SaveGameSubObjectData = SaveGameSpecification.GetSaveGameSubObjectData(OuterActor->GetClass(), Object->GetClass()))
						{
							FLudeoObjectStateManager::FindObjectToBeSavedFromProperty
							(
								WorldContextObject,
								Object->GetClass(),
								Object,
								SaveGameSubObjectData->SubObjectFilter.ObjectPropertyFilter,
								SaveGameSpecification,
								ObjectSet,
								HasVisitedObjectSet
							);
						}
						else
						{
							FLudeoObjectStateManager::FindObjectToBeSavedFromProperty
							(
								WorldContextObject,
								Object->GetClass(),
								Object,
								SaveGameSpecification.DefaultSubObjectSaveGameData.SubObjectFilter.ObjectPropertyFilter,
								SaveGameSpecification,
								ObjectSet,
								HasVisitedObjectSet
							);
						}
					}
				}
			}
		}
	};

	for (TFieldIterator<FProperty> PropertyIterator(StructureType); PropertyIterator; ++PropertyIterator)
	{
		FProperty* Property = *PropertyIterator;
		check(Property != nullptr);

		if (ObjectPropertyFilter.Match(*Property))
		{	
			if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
			{
				FScriptArrayHelper ScriptArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(StructureContainer));

				if (
					ArrayProperty->Inner->IsA(FStructProperty::StaticClass()) ||
					ArrayProperty->Inner->IsA(FObjectProperty::StaticClass())
				)
				{
					for (int32 i = 0; i < ScriptArrayHelper.Num(); ++i)
					{
						HandleNonContainerProperty(*ArrayProperty->Inner, ScriptArrayHelper.GetRawPtr(i));
					}
				}
			}
			else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
			{
				FScriptSetHelper ScriptSetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(StructureContainer));

				if (
					ScriptSetHelper.GetElementProperty()->IsA(FStructProperty::StaticClass()) ||
					ScriptSetHelper.GetElementProperty()->IsA(FObjectProperty::StaticClass())
				)
				{
					for (int32 i = 0; i < ScriptSetHelper.Num(); ++i)
					{
						HandleNonContainerProperty(*ScriptSetHelper.GetElementProperty(), ScriptSetHelper.GetElementPtr(i));
					}
				}
			}
			else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
			{
				FScriptMapHelper ScriptMapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(StructureContainer));

				if(
					ScriptMapHelper.GetKeyProperty()->IsA(FStructProperty::StaticClass()) || 
					ScriptMapHelper.GetKeyProperty()->IsA(FObjectProperty::StaticClass())
				)
				{
					for (int32 i = 0; i < ScriptMapHelper.Num(); ++i)
					{
						HandleNonContainerProperty(*ScriptMapHelper.GetKeyProperty(), ScriptMapHelper.GetKeyPtr(i));
					}
				}

				if (
					ScriptMapHelper.GetValueProperty()->IsA(FStructProperty::StaticClass()) ||
					ScriptMapHelper.GetValueProperty()->IsA(FObjectProperty::StaticClass())
				)
				{
					for (int32 i = 0; i < ScriptMapHelper.Num(); ++i)
					{
						HandleNonContainerProperty(*ScriptMapHelper.GetValueProperty(), ScriptMapHelper.GetValuePtr(i));
					}
				}
			}
			else
			{
				HandleNonContainerProperty(*Property, StructureContainer);
			}
		}
	}
}

const TSet<const UObject*>& FLudeoObjectStateManager::GetObjectToBeSavedSet
(
	const UObject* WorldContextObject,
	const TOptional<TArray<AActor*>>& OptionalSearchActorCollection,
	const FLudeoSaveGameSpecification& SaveGameSpecification
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	static TSet<const UObject*> ObjectSet;
	static TSet<const UObject*> HasVisitedObjectSet;

	ObjectSet.Empty(ObjectSet.Num());
	HasVisitedObjectSet.Empty(HasVisitedObjectSet.Num());

	const auto SearchActor = [&](const AActor* Actor)
	{
		check(Actor != nullptr);

		if(Actor != nullptr)
	{
			if (FLudeoObjectStateManager::IsValidObjectToBeSaved(World, Actor))
			{
				if (const FLudeoSaveGameActorData* SaveGameActorData = SaveGameSpecification.GetSaveGameActorData(Actor->GetClass()))
				{
					// Actor
					if (SaveGameActorData->Strategy == ELudeoSaveGameStrategy::Purge || SaveGameActorData->Strategy == ELudeoSaveGameStrategy::Reconcile)
					{
						HasVisitedObjectSet.Add(Actor);
						ObjectSet.Add(Actor);

						FLudeoObjectStateManager::FindObjectToBeSavedFromProperty
						(
							World,
							Actor->GetClass(),
							Actor,
							SaveGameActorData->ActorFilter.ActorPropertyFilter,
							SaveGameSpecification,
							ObjectSet,
							HasVisitedObjectSet
						);
					}
				}
			}
		}
	};

	if (OptionalSearchActorCollection.IsSet())
	{
		for (const AActor* Actor : OptionalSearchActorCollection.GetValue())
		{
			SearchActor(Actor);
		}
	}
	else
	{
		for (FActorIterator ActorItr(World); ActorItr; ++ActorItr)
		{
			SearchActor(*ActorItr);
		}
	}

	return ObjectSet;
}

const APlayerState* FLudeoObjectStateManager::GetObjectAssociatedPlayerState(const UObject* Object)
{
	check(Object != nullptr);

	if (const AActor* Actor = Cast<const AActor>(Object))
	{
		if (const APlayerController* PlayerControllerActor = Cast<const APlayerController>(Object))
		{
			return Cast<const APlayerState>(PlayerControllerActor->PlayerState);
		}
		else if (const APlayerState* PlayerStateActor = Cast<const APlayerState>(Object))
		{
			return PlayerStateActor;
		}
		else
		{
			if (APlayerController* OwnedByPlayerControllerActor = Cast<APlayerController>(Actor->GetOwner()))
			{
				return Cast<const APlayerState>(OwnedByPlayerControllerActor->PlayerState);
			}
			else if (const APlayerState* OwnedByPlayerStateActor = Cast<const APlayerState>(Actor->GetOwner()))
			{
				return OwnedByPlayerStateActor;
			}
		}
	}
	else if (const AActor* OuterActor = Object->GetTypedOuter<AActor>())
	{
		return GetObjectAssociatedPlayerState(OuterActor);
	}

	return static_cast<const APlayerState*>(nullptr);
}

TMap<TSubclassOf<AActor>, TArray<AActor*>> FLudeoObjectStateManager::GetActorClassMap(const UWorld* World)
{
	TMap<TSubclassOf<AActor>, TArray<AActor*>> ActorClassMap;

	for (TActorIterator<AActor> Itr(const_cast<UWorld*>(World)); Itr; ++Itr)
	{
		if (!Itr->IsPendingKillOrUnreachable())
		{
			TArray<AActor*>& ActorCollection = ActorClassMap.FindOrAdd(Itr->GetClass());

			ActorCollection.Add(*Itr);
		}
	}

	return ActorClassMap;
}

bool FLudeoObjectStateManager::SaveWorld
(
	const UObject* WorldContextObject,
	const FLudeoRoom& LudeoRoom,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	const TOptional<TArray<AActor*>>& OptionalSearchActorCollection,
	FLudeoWritableObject::WritableObjectMapType& CurrentObjectMap
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	FLudeoObjectStateManager::UpdateSaveWorldObjectMap(WorldContextObject, LudeoRoom, SaveGameSpecification, OptionalSearchActorCollection, CurrentObjectMap);

	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection = SaveGameSpecification.SaveGameActorDataCollection;

	bool bIsAllDataWrittenSuccessfully = true;

	for (
		FLudeoWritableObject::WritableObjectMapType::TConstIterator Itr = CurrentObjectMap.CreateConstIterator();
		(bIsAllDataWrittenSuccessfully && Itr);
		++Itr
	)
	{
		const UObject* Object = Itr->Get<0>();
		const FLudeoWritableObject& WritableObject = Itr->Get<1>();

		const FLudeoObjectPropertyFilter& PropertyFilter = [&]()
		{
			if (const AActor* Actor = Cast<const AActor>(Object))
			{
				const FLudeoSaveGameActorData* SaveGameActorData = SaveGameSpecification.GetSaveGameActorData(Actor->GetClass());
				check(SaveGameActorData != nullptr);

				if (SaveGameActorData != nullptr)
				{
					return SaveGameActorData->ActorFilter.ActorPropertyFilter;
				}
			}
			else
			{
				if(AActor* OuterActor = Object->GetTypedOuter<AActor>())
				{
					const FLudeoSaveGameSubObjectData* SaveGameSubObjectData = SaveGameSpecification.GetSaveGameSubObjectData
					(
						OuterActor->GetClass(),
						Object->GetClass()
					);

					if (SaveGameSubObjectData != nullptr)
					{
						return SaveGameSubObjectData->SubObjectFilter.ObjectPropertyFilter;
					}
				}
			}

			return SaveGameSpecification.DefaultSubObjectSaveGameData.SubObjectFilter.ObjectPropertyFilter;
		}();

		const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoWritableObject> EnterObjectGuard(WritableObject);

		if (const APlayerState* AssociatedPlayer = GetObjectAssociatedPlayerState(Object))
		{
			const FScopedWritableObjectBindPlayerGuard<FLudeoWritableObject> BindPlayerGuard
			(
				WritableObject,
				*FString::FromInt(AssociatedPlayer->GetPlayerId())
			);

			bIsAllDataWrittenSuccessfully = WritableObject.WriteData(CurrentObjectMap, PropertyFilter);
		}
		else
		{
			bIsAllDataWrittenSuccessfully = WritableObject.WriteData(CurrentObjectMap, PropertyFilter);
		}

		if (bIsAllDataWrittenSuccessfully)
		{
			const UObject* OuterObject = Object->GetOuter();
			check(OuterObject != nullptr);

			if (!OuterObject->IsA(ULevel::StaticClass()))
			{
				bIsAllDataWrittenSuccessfully = WritableObject.WriteData
				(
					TEXT("OuterPrivate"),
					static_cast<FLudeoObjectHandle>(CurrentObjectMap.FindChecked(OuterObject))
				);
			}
		}
	}

	return bIsAllDataWrittenSuccessfully;
}

bool FLudeoObjectStateManager::RestoreWorld
(
	const UObject* WorldContextObject,
	const TArray<FLudeoObjectInformation>& ObjectInformationCollection,
	const TArray<TSubclassOf<UObject>>& ObjectClassCollection,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	FLudeoReadableObject::ReadableObjectMapType& CurrentObjectMap
)
{
	FLudeoObjectStateManager::UpdateRestoreWorldObjectMap
	(
		WorldContextObject,
		ObjectInformationCollection,
		ObjectClassCollection,
		SaveGameSpecification,
		CurrentObjectMap
	);

	bool bIsAllDataReadSuccessfully = true;

	for (FLudeoReadableObject::ReadableObjectMapType::TConstIterator Itr = CurrentObjectMap.CreateConstIterator(); bIsAllDataReadSuccessfully && Itr; ++Itr)
	{
		const FLudeoReadableObject& ReadableObject = Itr->Get<0>();
		UObject* Object = const_cast<UObject*>(Itr->Get<1>());

		const FLudeoObjectPropertyFilter& PropertyFilter = [&]()
		{
			if (AActor* Actor = Cast<AActor>(Object))
			{
				const FLudeoSaveGameActorData* SaveGameActorData = SaveGameSpecification.GetSaveGameActorData(Actor->GetClass());

				check(SaveGameActorData != nullptr);

				if (SaveGameActorData != nullptr)
				{
					return SaveGameActorData->ActorFilter.ActorPropertyFilter;
				}
			}
			else
			{
				if(AActor* OuterActor = Object->GetTypedOuter<AActor>())
				{
					const FLudeoSaveGameSubObjectData* SaveGameSubObjectData = SaveGameSpecification.GetSaveGameSubObjectData
					(
						OuterActor->GetClass(),
						Object->GetClass()
					);

					if (SaveGameSubObjectData != nullptr)
					{
						return SaveGameSubObjectData->SubObjectFilter.ObjectPropertyFilter;
					}
				}
			}

			return SaveGameSpecification.DefaultSubObjectSaveGameData.SubObjectFilter.ObjectPropertyFilter;
		}();

		const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObject);

		bIsAllDataReadSuccessfully = ReadableObject.ReadData(Object, CurrentObjectMap, PropertyFilter);
	}

	return bIsAllDataReadSuccessfully;
}
