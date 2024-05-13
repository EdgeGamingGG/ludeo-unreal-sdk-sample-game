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
	FLudeoWritableObject::WritableObjectMapType& ObjectMap
)
{
	FLudeoRoomWriterCreateObjectParameters CreateObjectParameters;
	CreateObjectParameters.Object = Object;

	const TOptionalWithLudeoResult<FLudeoWritableObject> Result = LudeoRoom.GetRoomWriter().CreateObject(CreateObjectParameters);

	if(Result.IsSuccessful())
	{
		const FLudeoWritableObject& WritableObject = ObjectMap.Emplace(Object, Result.GetValue());

		return &WritableObject;
	}

	return nullptr;
}

bool FLudeoObjectStateManager::DestroyWritableObject
(
	const FLudeoRoom& LudeoRoom,
	const FLudeoWritableObject& WritableObject,
	FLudeoWritableObject::WritableObjectMapType& ObjectMap
)
{
	FLudeoRoomWriterDestroyObjectParameters DestroyObjectParameters;
	DestroyObjectParameters.ObjectHandle = WritableObject;

	const FLudeoResult Result = LudeoRoom.GetRoomWriter().DestroyObject(DestroyObjectParameters);
	
	if (Result.IsSuccessful())
	{
		ObjectMap.Remove(WritableObject.GetObject());
	}

	return Result.IsSuccessful();
}

void FLudeoObjectStateManager::CreateSaveWorldObjectMap
(
	const UObject* WorldContextObject,
	const FLudeoRoom& LudeoRoom,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	FLudeoWritableObject::WritableObjectMapType& ObjectMap
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	// Destroy objects
	if (ObjectMap.Num() > 0)
	{
		for (FLudeoWritableObject::WritableObjectMapType::TIterator Itr = ObjectMap.CreateIterator(); Itr; ++Itr)
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
	}

	// Create Object
	{
		const TSet<const UObject*> ThisFrameObjectSet = FLudeoObjectStateManager::GetObjectToBeSavedSet
		(
			World,
			SaveGameSpecification.SaveGameActorDataCollection
		);

		if (ThisFrameObjectSet.Num() > 0)
		{
			for (const UObject* Object : ThisFrameObjectSet)
			{
				if (!ObjectMap.Contains(Object))
				{
					const FLudeoWritableObject* WritableObject = CreateWritableObject(LudeoRoom, Object, ObjectMap);
					check(WritableObject != nullptr);
				}
			}
		}
	}
}

bool FLudeoObjectStateManager::CreateRestoreWorldObjectMap
(
	const UObject* WorldContextObject,
	const TArray<FLudeoObjectInformation>& ObjectInformationCollection,
	const TArray<TSubclassOf<UObject>>& ObjectClassCollection,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	FLudeoReadableObject::ReadableObjectMapType& ObjectMap
)
{
	check(ObjectInformationCollection.Num() == ObjectClassCollection.Num());

	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection = SaveGameSpecification.SaveGameActorDataCollection;

	if(SaveGameActorDataCollection.Num() > 0)
	{
		const auto GetActorSaveGameStrategry = [&](const UClass* ObjectClass)
		{
			check(ObjectClass != nullptr);
		
			if(ObjectClass->IsChildOf(AActor::StaticClass()))
			{
				const TSubclassOf<AActor> ActorClass(const_cast<UClass*>(ObjectClass));

				if (const FLudeoSaveGameActorData* SaveGameActorData = FLudeoObjectStateManager::GetSaveGameActorData(ActorClass, SaveGameActorDataCollection))
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
			if (GetActorSaveGameStrategry(Itr->GetClass()) == ELudeoSaveGameStrategy::Purge)
			{
				World->DestroyActor(*Itr);
			}
		}

		const FLudeoReadableObject::ReadableObjectMapType InitialObjectMap = ObjectMap;

		TMap<const UObject*, FLudeoReadableObject> InvertedObjectMap;
		{
			for (FLudeoReadableObject::ReadableObjectMapType::TConstIterator Itr = ObjectMap.CreateConstIterator(); Itr; ++Itr)
			{
				InvertedObjectMap.Emplace(Itr->Value, Itr->Key);
			}
		}

		{
			// Get an actor class map for reconciling
			const TMap<TSubclassOf<AActor>, TArray<const AActor*>> ActorClassMap = FLudeoObjectStateManager::GetActorClassMap(World);

			// Iterate through the object information collection
			for(int32 i = 0; i < ObjectInformationCollection.Num(); ++i)
			{
				const FLudeoReadableObject& ReadableObject = ObjectInformationCollection[i].ReadableObject;
				const TSubclassOf<UObject>& ObjectClass = ObjectClassCollection[i];

				// Skip the data that has already has a mapping
				if(!InitialObjectMap.Contains(ReadableObject))
				{
					const AActor* Actor = [&]()
					{
						// Create mapping for actor object first
						if(ObjectClass->IsChildOf(AActor::StaticClass()))
						{
							const ELudeoSaveGameStrategy SaveGameStrategy = GetActorSaveGameStrategry(ObjectClass);

							if(SaveGameStrategy == ELudeoSaveGameStrategy::Reconcile)
							{
								const TArray<const AActor*>* pObjectCollection = ActorClassMap.Find(ObjectClass.Get());
								check(pObjectCollection != nullptr);

								if(pObjectCollection != nullptr)
								{
									const TArray<const AActor*> ActorCollection = pObjectCollection->FilterByPredicate([&](const AActor* Actor)
									{
										return !InvertedObjectMap.Contains(Actor);
									});
									check(ActorCollection.Num() == 1);

									return Cast<const AActor>(ActorCollection.Last());
								}
							}
							else if(SaveGameStrategy == ELudeoSaveGameStrategy::Purge)
							{
								FActorSpawnParameters ActorSpawnParameters;
								ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

								const AActor* Actor = World->SpawnActor<AActor>(ObjectClass, ActorSpawnParameters);
								check(Actor != nullptr);

								// Destroy all actor components that has to be purged
								for (TFieldIterator<FObjectProperty> PropertyIterator(Actor->GetClass()); PropertyIterator; ++PropertyIterator)
								{
									FObjectProperty* ObjectProperty = *PropertyIterator;
									check(ObjectProperty != nullptr);

									if (UActorComponent* ActorComponent = Cast<UActorComponent>(ObjectProperty->GetObjectPropertyValue_InContainer(const_cast<AActor*>(Actor))))
									{
										if (const FLudeoSaveGameActorCompomnentData* SaveGameActorCompomnentData = FLudeoObjectStateManager::GetSaveGameActorComponentData(Actor->GetClass(), ActorComponent->GetClass(), SaveGameActorDataCollection))
										{
											if (SaveGameActorCompomnentData->Strategy == ELudeoSaveGameStrategy::Purge)
											{
												ActorComponent->DestroyComponent(true);
												ObjectProperty->SetObjectPropertyValue_InContainer(const_cast<AActor*>(Actor), nullptr);
											}
										}
									}
								}

								return Actor;
							}
							else
							{
								// The object exists in the Ludeo, but the game has already removed it (Ludeo Compatibility Issue)
								check(!ObjectClass->IsChildOf(AActor::StaticClass()));
							}

							check(false);
						}

						return static_cast<const AActor*>(nullptr);
					}();

					if(Actor != nullptr)
					{
						check(Actor->GetClass() == ObjectClass);

						ObjectMap.Emplace(ReadableObject, Actor);
						InvertedObjectMap.Emplace(Actor, ReadableObject);
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
				const UObject* Object = [&]()
				{
					// Create mapping for the non-actor object
					if(!ObjectClass->IsChildOf(AActor::StaticClass()))
					{
						bool bHasReadSuccessfully = true;

						const FLudeoObjectHandle OuterLudeoObjectHandle = GetOuterObjectLudeoObjectHandle(ReadableObject);

						if(const UObject* const* OuterObject = ObjectMap.FindByHash(GetTypeHash(OuterLudeoObjectHandle), OuterLudeoObjectHandle))
						{
							const FLudeoSaveGameActorCompomnentData* SaveGameActorCompomnentData = [&]()
							{
								if(const AActor* OuterActor = Cast<AActor>(const_cast<UObject*>(*OuterObject)))
								{
									if (ObjectClass != nullptr && ObjectClass->IsChildOf(UActorComponent::StaticClass()))
									{
										return FLudeoObjectStateManager::GetSaveGameActorComponentData
										(
											OuterActor->GetClass(),
											ObjectClass.Get(),
											SaveGameActorDataCollection
										);
									}
								}

								return static_cast<const FLudeoSaveGameActorCompomnentData*>(nullptr);
							}();

							if (SaveGameActorCompomnentData != nullptr)
							{
								const AActor* OuterActor = Cast<AActor>(const_cast<UObject*>(*OuterObject));

								if (SaveGameActorCompomnentData->Strategy == ELudeoSaveGameStrategy::Purge)
								{
									return NewObject<UObject>(const_cast<AActor*>(OuterActor), ObjectClass);
								}
								else if (SaveGameActorCompomnentData->Strategy == ELudeoSaveGameStrategy::Reconcile)
								{
									const FLudeoReadableObject& OuterActorReadableObject = InvertedObjectMap.FindChecked(OuterActor);

									const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(OuterActorReadableObject);

									for (TFieldIterator<FObjectProperty> PropertyIterator(OuterActor->GetClass()); PropertyIterator; ++PropertyIterator)
									{
										FObjectProperty* ObjectProperty = *PropertyIterator;
										check(ObjectProperty != nullptr);

										if(ObjectProperty->PropertyClass->IsChildOf(UActorComponent::StaticClass()))
										{
											FLudeoObjectHandle LudeoObjectHandle;
								
											if (OuterActorReadableObject.ReadData(*ObjectProperty->GetName(), LudeoObjectHandle))
											{
												if (LudeoObjectHandle == static_cast<FLudeoObjectHandle>(ReadableObject))
												{
													UObject* Object = ObjectProperty->GetObjectPropertyValue_InContainer(OuterActor);
													check(Object != nullptr);

													return Object;
												}
											}
										}
									}
								}
						
								check(false);
							}
							else
							{
								return NewObject<UObject>(const_cast<UObject*>(*OuterObject), ObjectClass);
							}
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

					ObjectMap.Emplace(ReadableObject, Object);
					InvertedObjectMap.Emplace(Object, ReadableObject);
				}
			}
		}

		// Restore outer of object created
		for (const TPair<const UObject*, FLudeoReadableObject>& Pair : InvertedObjectMap)
		{
			UObject* const& Object = const_cast<UObject*>(Pair.Get<0>());

			if(Object->GetOuter() == GetTransientPackage())
			{
				const FLudeoObjectHandle OuterLudeoObjectHandle = GetOuterObjectLudeoObjectHandle(Pair.Get<1>());
					
				const UObject* const* OuterObject = ObjectMap.FindByHash(GetTypeHash(OuterLudeoObjectHandle), OuterLudeoObjectHandle);
				check(OuterObject != nullptr);

				Object->Rename(nullptr, const_cast<UObject*>(*OuterObject));
			}
		}
	}

	return true;
}

bool FLudeoObjectStateManager::IsTargetObjectToBeSaved(const UObject* WorldContextObject, const UObject* Object)
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
	const void* MemoryAddress,
	TSet<const UObject*>& ObjectSet,
	TSet<const UObject*>& HasVisitedObjectSet
)
{
	for (TFieldIterator<FProperty> PropertyIterator(StructureType); PropertyIterator; ++PropertyIterator)
	{
		FProperty* Property = *PropertyIterator;
		check(Property != nullptr);

		if (Property->HasAllPropertyFlags(EPropertyFlags::CPF_SaveGame))
		{
			if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
			{
				FLudeoObjectStateManager::FindObjectToBeSavedFromProperty
				(
					WorldContextObject,
					StructProperty->Struct,
					StructProperty->ContainerPtrToValuePtr<void>(MemoryAddress),
					ObjectSet,
					HasVisitedObjectSet
				);
			}
			else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
			{
				const UObject* Object = ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(MemoryAddress));

				if (Object != nullptr)
				{
					if (!HasVisitedObjectSet.Contains(Object))
					{
						HasVisitedObjectSet.Add(Object);

						if (FLudeoObjectStateManager::IsTargetObjectToBeSaved(WorldContextObject, Object))
						{
							ObjectSet.Add(Object);
						}

						FLudeoObjectStateManager::FindObjectToBeSavedFromProperty
						(
							WorldContextObject,
							Object->GetClass(),
							Object,
							ObjectSet,
							HasVisitedObjectSet
						);
					}
				}
			}
		}
	}
}

TSet<const UObject*> FLudeoObjectStateManager::GetObjectToBeSavedSet
(
	const UObject* WorldContextObject,
	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	TSet<const UObject*> ObjectSet;
	TSet<const UObject*> HasVisitedObjectSet;

	for (FActorIterator ActorItr(World); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		check(Actor != nullptr);

		if (ULevel* Outer = Cast<ULevel>(Actor->GetOuter()))
		{
			if(FLudeoObjectStateManager::IsTargetObjectToBeSaved(World, Actor))
			{
				if (const FLudeoSaveGameActorData* SaveGameActorData = FLudeoObjectStateManager::GetSaveGameActorData(Actor->GetClass(), SaveGameActorDataCollection))
				{
					// Actor
					if(SaveGameActorData->Strategy == ELudeoSaveGameStrategy::Purge || SaveGameActorData->Strategy == ELudeoSaveGameStrategy::Reconcile)
					{
						HasVisitedObjectSet.Add(Actor);
						ObjectSet.Add(Actor);

						FLudeoObjectStateManager::FindObjectToBeSavedFromProperty(World, Actor->GetClass(), Actor, ObjectSet, HasVisitedObjectSet);
					}

					if(SaveGameActorData->SaveGameActorComponentDataCollection.Num() > 0)
					{
						for (TFieldIterator<FObjectProperty> PropertyIterator(Actor->GetClass()); PropertyIterator; ++PropertyIterator)
						{
							FObjectProperty* ObjectProperty = *PropertyIterator;
							check(ObjectProperty != nullptr);

							const UActorComponent* ActorComponent = Cast<UActorComponent>(ObjectProperty->GetObjectPropertyValue(ObjectProperty->ContainerPtrToValuePtr<void>(Actor)));

							if(ActorComponent != nullptr)
							{
								if (const FLudeoSaveGameActorCompomnentData* SaveGameActorCompomnentData = FLudeoObjectStateManager::GetSaveGameActorComponentData(ActorComponent->GetClass(), SaveGameActorData->SaveGameActorComponentDataCollection))
								{
									if (SaveGameActorCompomnentData->Strategy == ELudeoSaveGameStrategy::Purge || SaveGameActorCompomnentData->Strategy == ELudeoSaveGameStrategy::Reconcile)
									{
										HasVisitedObjectSet.Add(ActorComponent);
										ObjectSet.Add(ActorComponent);

										FLudeoObjectStateManager::FindObjectToBeSavedFromProperty(World, ActorComponent->GetClass(), ActorComponent, ObjectSet, HasVisitedObjectSet);
									}
								}
							}
						}
					}
				}
			}
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

TMap<TSubclassOf<AActor>, TArray<const AActor*>> FLudeoObjectStateManager::GetActorClassMap(const UWorld* World)
{
	TMap<TSubclassOf<AActor>, TArray<const AActor*>> ActorClassMap;

	for (TActorIterator<AActor> Itr(const_cast<UWorld*>(World)); Itr; ++Itr)
	{
		if (!Itr->IsPendingKillOrUnreachable())
		{
			TArray<const AActor*>& ActorCollection = ActorClassMap.FindOrAdd(Itr->GetClass());

			ActorCollection.Add(*Itr);
		}
	}

	return ActorClassMap;
}

const FLudeoSaveGameActorData* FLudeoObjectStateManager::GetSaveGameActorData
(
	const TSubclassOf<AActor>& ActorClass,
	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection
)
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

const FLudeoSaveGameActorCompomnentData* FLudeoObjectStateManager::GetSaveGameActorComponentData
(
	const TSubclassOf<UActorComponent>& ActorComponentClass,
	const TArray<FLudeoSaveGameActorCompomnentData>& SaveGameActorComponentDataCollection
)
{
	const int32 Index = SaveGameActorComponentDataCollection.FindLastByPredicate
	(
		[&](const FLudeoSaveGameActorCompomnentData& SaveGameActorComponentData)
		{
			return SaveGameActorComponentData.ActorCompomnentFilter.Match(ActorComponentClass);
		}
	);

	if (SaveGameActorComponentDataCollection.IsValidIndex(Index))
	{
		return &SaveGameActorComponentDataCollection[Index];
	}

	return nullptr;
}

const FLudeoSaveGameActorCompomnentData* FLudeoObjectStateManager::GetSaveGameActorComponentData
(
	const TSubclassOf<AActor>& OuterActorClass,
	const TSubclassOf<UActorComponent>& ActorComponentClass,
	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection
)
{
	check(ActorComponentClass != nullptr);

	if (const FLudeoSaveGameActorData* SaveGameActorData = FLudeoObjectStateManager::GetSaveGameActorData(OuterActorClass, SaveGameActorDataCollection))
	{
		return GetSaveGameActorComponentData(ActorComponentClass, SaveGameActorData->SaveGameActorComponentDataCollection);
	}

	return nullptr;
}

bool FLudeoObjectStateManager::SaveWorld
(
	const UObject* WorldContextObject,
	const TOptional<FString> LudeoCreatorPlayerID,
	const FLudeoRoom& LudeoRoom,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	FLudeoWritableObject::WritableObjectMapType& ObjectMap
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	FLudeoObjectStateManager::CreateSaveWorldObjectMap(WorldContextObject, LudeoRoom, SaveGameSpecification, ObjectMap);

	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection = SaveGameSpecification.SaveGameActorDataCollection;

	const auto SaveAdditionalSaveGameProperty = [&](const FLudeoWritableObject& WritableObject, const UObject* Object, const TArray<FName>& AdditionalSaveGamePropertyCollection)
	{
		check(Object != nullptr);

		bool bIsAllDataWrittenSuccessfully = true;

		for (const FName& AdditionalSaveGameProperty : AdditionalSaveGamePropertyCollection)
		{
			FProperty* Property = Object->GetClass()->FindPropertyByName(AdditionalSaveGameProperty);

			if (ensure(Property != nullptr))
			{
				bIsAllDataWrittenSuccessfully = WritableObject.WriteData
				(
					*Property->GetName(),
					Object,
					Property,
					ObjectMap
				);

				check(bIsAllDataWrittenSuccessfully);
			}
		}

		return bIsAllDataWrittenSuccessfully;
	};

	bool bIsAllDataWrittenSuccessfully = true;

	for (
		FLudeoWritableObject::WritableObjectMapType::TConstIterator Itr = ObjectMap.CreateConstIterator();
		(bIsAllDataWrittenSuccessfully && Itr);
		++Itr
	)
	{
		const UObject* Object = Itr->Get<0>();
		const FLudeoWritableObject& WritableObject = Itr->Get<1>();

		const auto SaveObject = [&]()
		{
			bool bSaveObjectSuccessfully = WritableObject.WriteData(ObjectMap);

			if (const AActor* Actor = Cast<AActor>(Object))
			{
				if (const FLudeoSaveGameActorData* SaveGameActorData = FLudeoObjectStateManager::GetSaveGameActorData(Actor->GetClass(), SaveGameActorDataCollection))
				{
					bIsAllDataWrittenSuccessfully = SaveAdditionalSaveGameProperty(WritableObject, Actor, SaveGameActorData->AdditionalSaveGamePropertyNameCollection);

					for (TFieldIterator<FObjectProperty> PropertyIterator(Actor->GetClass()); bIsAllDataWrittenSuccessfully && PropertyIterator; ++PropertyIterator)
					{
						const FObjectProperty* ObjectProperty = *PropertyIterator;
						check(ObjectProperty != nullptr);

						if (const UActorComponent* ActorComponent = Cast<UActorComponent>(ObjectProperty->GetObjectPropertyValue_InContainer(Actor)))
						{
							if (const FLudeoSaveGameActorCompomnentData* SaveGameActorComponentData = FLudeoObjectStateManager::GetSaveGameActorComponentData(ActorComponent->GetClass(), SaveGameActorData->SaveGameActorComponentDataCollection))
							{
								if (
									SaveGameActorComponentData->Strategy == ELudeoSaveGameStrategy::Purge		||
									SaveGameActorComponentData->Strategy == ELudeoSaveGameStrategy::Reconcile
								)
								{
									bSaveObjectSuccessfully = WritableObject.WriteData(*ObjectProperty->GetName(), ActorComponent, ObjectMap);
								}
							}
						}
					}
				}
				else
				{
					check(false);
				}
			}
			else if (const UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
			{
				if (const FLudeoSaveGameActorCompomnentData* SaveGameActorComponentData = FLudeoObjectStateManager::GetSaveGameActorComponentData(ActorComponent->GetOuter()->GetClass(), ActorComponent->GetClass(), SaveGameActorDataCollection))
				{
					if (SaveGameActorComponentData->Strategy == ELudeoSaveGameStrategy::Purge || SaveGameActorComponentData->Strategy == ELudeoSaveGameStrategy::Reconcile)
					{
						bIsAllDataWrittenSuccessfully = SaveAdditionalSaveGameProperty(WritableObject, ActorComponent, SaveGameActorComponentData->AdditionalSaveGamePropertyNameCollection);
					}
				}
			}

			return bSaveObjectSuccessfully;
		};

		const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoWritableObject> EnterObjectGuard(WritableObject);

		if (const APlayerState* AssociatedPlayer = GetObjectAssociatedPlayerState(Object))
		{
			const bool bShouldReplacePlayerIDWithCreatorPlayerID =
			(
				(AssociatedPlayer->GetOwner() == World->GetFirstPlayerController()) &&
				LudeoCreatorPlayerID.IsSet() &&
				!LudeoCreatorPlayerID.GetValue().IsEmpty()
			);

			const FScopedWritableObjectBindPlayerGuard<FLudeoWritableObject> BindPlayerGuard
			(
				WritableObject,
				(bShouldReplacePlayerIDWithCreatorPlayerID ? *LudeoCreatorPlayerID.GetValue() : *AssociatedPlayer->GetUniqueId().ToString())
			);

			bIsAllDataWrittenSuccessfully = SaveObject();
		}
		else
		{
			bIsAllDataWrittenSuccessfully = SaveObject();
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
					static_cast<FLudeoObjectHandle>(ObjectMap.FindChecked(OuterObject))
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
	const FLudeoReadableObject::ReadableObjectMapType& InputObjectMap
)
{
	FLudeoReadableObject::ReadableObjectMapType ObjectMap = InputObjectMap;

	const auto RestoreAdditionalSaveGameProperty = [&]
	(
		const FLudeoReadableObject& ReadableObject,
		const UObject* Object,
		const TArray<FName>& AdditionalSaveGamePropertyCollection
	)
	{
		bool bIsAllDataReadSuccessfully = true;

		for (const FName& AdditionalSaveGameProperty : AdditionalSaveGamePropertyCollection)
		{
			FProperty* Property = Object->GetClass()->FindPropertyByName(AdditionalSaveGameProperty);

			if (ensureAlways(Property != nullptr))
			{
				const FString PropertyName = Property->GetName();

				bIsAllDataReadSuccessfully = ReadableObject.ReadData
				(
					*PropertyName,
					Object,
					Property,
					ObjectMap
				);

				// It can happen that attribute exists locally, but has not written to the server
				bIsAllDataReadSuccessfully = true;
			}
		}

		return bIsAllDataReadSuccessfully;
	};

	FLudeoObjectStateManager::CreateRestoreWorldObjectMap
	(
		WorldContextObject,
		ObjectInformationCollection,
		ObjectClassCollection,
		SaveGameSpecification,
		ObjectMap
	);

	bool bIsAllDataReadSuccessfully = true;

	for (FLudeoReadableObject::ReadableObjectMapType::TConstIterator Itr = ObjectMap.CreateConstIterator(); bIsAllDataReadSuccessfully && Itr; ++Itr)
	{
		const FLudeoReadableObject& ReadableObject = Itr->Get<0>();
		UObject* Object = const_cast<UObject*>(Itr->Get<1>());

		const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObject);

		bIsAllDataReadSuccessfully = ReadableObject.ReadData(Object, ObjectMap);

		if(bIsAllDataReadSuccessfully)
		{
			if (AActor* Actor = Cast<AActor>(Object))
			{
				const FLudeoSaveGameActorData* SaveGameActorData = FLudeoObjectStateManager::GetSaveGameActorData
				(
					Actor->GetClass(),
					SaveGameSpecification.SaveGameActorDataCollection
				);
				check(SaveGameActorData != nullptr);

				if (SaveGameActorData != nullptr)
				{
					RestoreAdditionalSaveGameProperty(ReadableObject, Actor, SaveGameActorData->AdditionalSaveGamePropertyNameCollection);

					for (TFieldIterator<FObjectProperty> PropertyIterator(Actor->GetClass()); PropertyIterator; ++PropertyIterator)
					{
						FObjectProperty* ObjectProperty = *PropertyIterator;
						check(ObjectProperty != nullptr);

						if (ObjectProperty->PropertyClass->IsChildOf(UActorComponent::StaticClass()))
						{
							FLudeoObjectHandle LudeoObjectHandle;
							
							if(ReadableObject.ReadData(*ObjectProperty->GetName(), LudeoObjectHandle))
							{
								if (LudeoObjectHandle == LUDEO_INVALID_OBJECTID)
								{
									ObjectProperty->SetObjectPropertyValue_InContainer(Actor, nullptr);
								}
								else
								{
									UObject* ObjectValue = const_cast<UObject*>(*ObjectMap.FindByHash(GetTypeHash(LudeoObjectHandle), LudeoObjectHandle));

									check(ObjectValue != nullptr);
									check(ObjectValue->GetClass()->IsChildOf(ObjectProperty->PropertyClass));

									ObjectProperty->SetObjectPropertyValue_InContainer(Actor, ObjectValue);
								}
							}
						}
					}
				}
			}
			else if (UActorComponent* ActorComponent = Cast<UActorComponent>(Object))
			{
				AActor* OuterActor = Cast<AActor>(ActorComponent->GetOuter());
				check(OuterActor != nullptr);

				if(OuterActor != nullptr)
				{
					if (const FLudeoSaveGameActorCompomnentData* SaveGameActorCompomnentData = FLudeoObjectStateManager::GetSaveGameActorComponentData(OuterActor->GetClass(), ActorComponent->GetClass(), SaveGameSpecification.SaveGameActorDataCollection))
					{
						if (USceneComponent* SceneComponent = Cast<USceneComponent>(Object))
						{
							if (
								SaveGameActorCompomnentData->AdditionalSaveGamePropertyNameCollection.Contains(SceneComponent->GetRelativeLocationPropertyName()) ||
								SaveGameActorCompomnentData->AdditionalSaveGamePropertyNameCollection.Contains(SceneComponent->GetRelativeRotationPropertyName()) ||
								SaveGameActorCompomnentData->AdditionalSaveGamePropertyNameCollection.Contains(SceneComponent->GetRelativeScale3DPropertyName())
							)
							{
								if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(SceneComponent))
								{
									if (PrimitiveComponent->IsSimulatingPhysics())
									{
										PrimitiveComponent->SetSimulatePhysics(true);
									}
								}

								RestoreAdditionalSaveGameProperty(ReadableObject, ActorComponent, SaveGameActorCompomnentData->AdditionalSaveGamePropertyNameCollection);
								SceneComponent->UpdateComponentToWorld(EUpdateTransformFlags::None, ETeleportType::TeleportPhysics);
							}
						}
						else
						{
							RestoreAdditionalSaveGameProperty(ReadableObject, ActorComponent, SaveGameActorCompomnentData->AdditionalSaveGamePropertyNameCollection);
						}
					}
				}
			}
		}
	}

	return bIsAllDataReadSuccessfully;
}
