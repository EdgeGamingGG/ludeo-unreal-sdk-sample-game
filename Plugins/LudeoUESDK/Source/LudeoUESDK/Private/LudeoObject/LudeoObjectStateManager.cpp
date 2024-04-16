#include "LudeoObject/LudeoObjectStateManager.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "LudeoScopedGuard.h"

#include "LudeoRoom/LudeoRoom.h"

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

bool FLudeoObjectStateManager::TickUpdateObjectState
(
	const FLudeoRoom& LudeoRoom,
	const FLudeoObjectStateManager::PlayerMapType& PlayerMap,
	const FLudeoSaveGameSpecification& SaveGameSpecification
)
{
	const APlayerState* WorldContextObject = [&]()
	{
		for (FLudeoObjectStateManager::PlayerMapType::TConstIterator Itr = PlayerMap.CreateConstIterator(); Itr; ++Itr)
		{
			return Itr->Get<0>();
		}

		return static_cast<const APlayerState*>(nullptr);
	}();

	if(WorldContextObject != nullptr)
	{
		FLudeoObjectStateManager::CreateSaveWorldObjectMap(LudeoRoom, WorldContextObject, SaveGameSpecification, TrackedLudeoObjectMap);

		return FLudeoObjectStateManager::SaveWorld(LudeoRoom, TrackedLudeoObjectMap, PlayerMap, SaveGameSpecification);
	}

	return false;
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

const FLudeoPlayer* FLudeoObjectStateManager::GetObjectAssociatedPlayer(const UObject* Object, const FLudeoObjectStateManager::PlayerMapType& PlayerMap)
{
	check(Object != nullptr);

	UWorld* World = Object->GetWorld();
	check(World != nullptr);

	AGameStateBase* GameState = World->GetGameState();
	check(GameState != nullptr);

	if(const AActor* Actor = Cast<const AActor>(Object))
	{
		if(const APlayerState* AssociatedPlayerState = GetObjectAssociatedPlayerState(Object))
		{
			const FLudeoPlayerHandle* PlayerHandle = PlayerMap.Find(AssociatedPlayerState);

			if (PlayerHandle != nullptr)
			{
				return FLudeoPlayer::GetPlayerByPlayerHandle(*PlayerHandle);
			}
		}
	}

	return nullptr;
}

bool FLudeoObjectStateManager::SaveWorld
(
	const FLudeoRoom& LudeoRoom,
	const FLudeoWritableObject::WritableObjectMapType& ObjectMap,
	const FLudeoObjectStateManager::PlayerMapType& PlayerMap,
	const FLudeoSaveGameSpecification& SaveGameSpecification
)
{
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

		if (const FLudeoPlayer* AssociatedPlayer = GetObjectAssociatedPlayer(Object, PlayerMap))
		{
			const FScopedWritableObjectBindPlayerGuard<FLudeoWritableObject> BindPlayerGuard(WritableObject, *AssociatedPlayer);
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

	if (bIsAllDataWrittenSuccessfully)
	{
		const UWorld* World = [&]()
		{
			if (ObjectMap.begin() != ObjectMap.end())
			{
				const UObject* WorldContextObject = ObjectMap.begin().Key();
				check(WorldContextObject != nullptr);

				return WorldContextObject->GetWorld();
			}

			return static_cast<UWorld*>(nullptr);
		}();
		
		if(AGameStateBase* GameState = (World != nullptr ? World->GetGameState() : nullptr))
		{
			for (int32 i = 0; (bIsAllDataWrittenSuccessfully && i < GameState->PlayerArray.Num()); ++i)
			{
				const APlayerState* PlayerState = GameState->PlayerArray[i];
				check(PlayerState != nullptr);

				if (const FLudeoWritableObject* WritableObject = ObjectMap.Find(PlayerState))
				{
					const FLudeoPlayer* AssociatedPlayer = GetObjectAssociatedPlayer(PlayerState, PlayerMap);
					check(AssociatedPlayer != nullptr);

					if(AssociatedPlayer != nullptr)
					{
						const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoWritableObject> EnterObjectGuard(*WritableObject);
						const FScopedWritableObjectBindPlayerGuard<FLudeoWritableObject> BindPlayerGuard(*WritableObject, *AssociatedPlayer);

						bIsAllDataWrittenSuccessfully = WritableObject->WriteData(TEXT("PlayerID"), AssociatedPlayer->GetPlayerID());
					}
				}
			}

			for (FConstPlayerControllerIterator Itr = World->GetPlayerControllerIterator(); (bIsAllDataWrittenSuccessfully && Itr); ++Itr)
			{
				if(const APlayerController* PlayerController = Itr->Get())
				{
					if (const FLudeoWritableObject* WritableObject = ObjectMap.Find(PlayerController))
					{
						const FLudeoPlayer* AssociatedPlayer = GetObjectAssociatedPlayer(PlayerController, PlayerMap);
						check(AssociatedPlayer != nullptr);

						if(AssociatedPlayer != nullptr)
						{
							const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoWritableObject> EnterObjectGuard(*WritableObject);
							const FScopedWritableObjectBindPlayerGuard<FLudeoWritableObject> BindPlayerGuard(*WritableObject, *AssociatedPlayer);
							
							bIsAllDataWrittenSuccessfully = WritableObject->WriteData(TEXT("PlayerID"), AssociatedPlayer->GetPlayerID());
						}
					}
				}
			}
		}
	}

	return bIsAllDataWrittenSuccessfully;
}

void FLudeoObjectStateManager::CreateSaveWorldObjectMap
(
	const FLudeoRoom& LudeoRoom,
	const UObject* WorldContextObject,
	const FLudeoSaveGameSpecification& SaveGameSpecification,
	FLudeoWritableObject::WritableObjectMapType& OutTrackedLudeoObjectMap
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	// Destroy objects
	if (OutTrackedLudeoObjectMap.Num() > 0)
	{
		for (FLudeoWritableObject::WritableObjectMapType::TIterator Itr = OutTrackedLudeoObjectMap.CreateIterator(); Itr; ++Itr)
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
			FLudeoRoomWriterCreateObjectParameters CreateObjectParameters;

			for (const UObject* Object : ThisFrameObjectSet)
			{
				if (!OutTrackedLudeoObjectMap.Contains(Object))
				{
					CreateObjectParameters.Object = Object;

					const TOptionalWithLudeoResult<FLudeoWritableObject> Result = LudeoRoom.GetRoomWriter().CreateObject(CreateObjectParameters);
					check(Result.IsSuccessful());

					OutTrackedLudeoObjectMap.Emplace(Object, Result.GetValue());
				}
			}
		}
	}
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

FLudeoReadableObject::ReadableObjectMapType FLudeoObjectStateManager::CreateRestoreWorldObjectMap
(
	const FLudeo& Ludeo,
	const UObject* WorldContextObject,
	const FLudeoSaveGameSpecification& SaveGameSpecification
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	const TArray<FLudeoSaveGameActorData>& SaveGameActorDataCollection = SaveGameSpecification.SaveGameActorDataCollection;

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

	FLudeoReadableObject::ReadableObjectMapType ObjectMap;
	TMap<const UObject*, FLudeoReadableObject> ObjectReadableObjectMap;
	TArray<const UObject*> ObjectWithTransientOuterCollection;

	const FString& LudeoCreatorPlayerID = Ludeo.GetCreatorPlayerID();

	const TArray<FLudeoObjectInformation>& ObjectInformationCollection = Ludeo.GetLudeoObjectInformationCollection();

	for (TActorIterator<AActor> Itr(World); Itr; ++Itr)
	{
		if (GetActorSaveGameStrategry(Itr->GetClass()) == ELudeoSaveGameStrategy::Purge)
		{
			World->DestroyActor(*Itr);
		}
	}

	for(const FLudeoObjectInformation& ObjectInformation : ObjectInformationCollection)
	{
		const AActor* Actor = [&]()
		{
			if(ObjectInformation.ObjectClass->IsChildOf(AActor::StaticClass()))
			{
				const ELudeoSaveGameStrategy SaveGameStrategy = GetActorSaveGameStrategry(ObjectInformation.ObjectClass);

				if(SaveGameStrategy == ELudeoSaveGameStrategy::Reconcile)
				{
					const TArray<const UObject*> ObjectCollection = FLudeoObjectStateManager::GetObjectsOfClass(World, ObjectInformation.ObjectClass);
					check(ObjectCollection.Num() == 1);

					if (
						ObjectInformation.ObjectClass->IsChildOf(APlayerController::StaticClass()) ||
						ObjectInformation.ObjectClass->IsChildOf(APlayerState::StaticClass())
					)
					{
						const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ObjectInformation.ReadableObject);

						FString PlayerID;

						if (ObjectInformation.ReadableObject.ReadData(TEXT("PlayerID"), PlayerID))
						{
							if (PlayerID == LudeoCreatorPlayerID)
							{
								return Cast<const AActor>(ObjectCollection.Last());
							}
						}
						else
						{
							check(false);
						}
					}
					else
					{
						return Cast<const AActor>(ObjectCollection.Last());
					}
				}
				else if(SaveGameStrategy == ELudeoSaveGameStrategy::Purge)
				{
					FActorSpawnParameters ActorSpawnParameters;
					ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					const AActor* Actor = World->SpawnActor<AActor>(ObjectInformation.ObjectClass, ActorSpawnParameters);
					check(Actor != nullptr);

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
					check(!ObjectInformation.ObjectClass->IsChildOf(AActor::StaticClass()));
				}

				check(false);
			}

			return static_cast<const AActor*>(nullptr);
		}();

		if(Actor != nullptr)
		{
			check(Actor->GetClass() == ObjectInformation.ObjectClass);

			ObjectMap.Emplace(ObjectInformation.ReadableObject, Actor);
			ObjectReadableObjectMap.Emplace(Actor, ObjectInformation.ReadableObject);
		}
	}

	for(const FLudeoObjectInformation& ObjectInformation : ObjectInformationCollection)
	{
		const UObject* Object = [&]()
		{
			if(!ObjectInformation.ObjectClass->IsChildOf(AActor::StaticClass()))
			{
				bool bHasReadSuccessfully = true;

				const FLudeoObjectHandle OuterLudeoObjectHandle = GetOuterObjectLudeoObjectHandle(ObjectInformation.ReadableObject);

				if(const UObject* const* OuterObject = ObjectMap.FindByHash(GetTypeHash(OuterLudeoObjectHandle), OuterLudeoObjectHandle))
				{
					const AActor* OuterActor = Cast<AActor>(const_cast<UObject*>(*OuterObject));
					check(OuterActor != nullptr);

					if (const FLudeoSaveGameActorCompomnentData* SaveGameActorCompomnentData = FLudeoObjectStateManager::GetSaveGameActorComponentData(OuterActor->GetClass(), ObjectInformation.ObjectClass.Get(), SaveGameActorDataCollection))
					{
						const FLudeoReadableObject& ReadableObject = ObjectReadableObjectMap.FindChecked(OuterActor);

						const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(ReadableObject);

						for (TFieldIterator<FObjectProperty> PropertyIterator(OuterActor->GetClass()); PropertyIterator; ++PropertyIterator)
						{
							FObjectProperty* ObjectProperty = *PropertyIterator;
							check(ObjectProperty != nullptr);

							if (ObjectInformation.ObjectClass->IsChildOf(ObjectProperty->PropertyClass))
							{
								FLudeoObjectHandle LudeoObjectHandle;
								bHasReadSuccessfully = ReadableObject.ReadData(*ObjectProperty->GetName(), LudeoObjectHandle);
								check(bHasReadSuccessfully);

								if (LudeoObjectHandle == static_cast<FLudeoObjectHandle>(ObjectInformation.ReadableObject))
								{
									if (SaveGameActorCompomnentData->Strategy == ELudeoSaveGameStrategy::Reconcile)
									{
										UObject* Object = ObjectProperty->GetObjectPropertyValue_InContainer(OuterActor);
										check(Object != nullptr);

										return Object;
									}
									else if (SaveGameActorCompomnentData->Strategy == ELudeoSaveGameStrategy::Purge)
									{
										return NewObject<UObject>(const_cast<AActor*>(OuterActor), ObjectInformation.ObjectClass);
									}
								}
							}
						}

						check(false);
					}
					else
					{
						return NewObject<UObject>(const_cast<UObject*>(*OuterObject), ObjectInformation.ObjectClass);
					}
				}
				else
				{
					return NewObject<UObject>(GetTransientPackage(), ObjectInformation.ObjectClass);
				}
			}

			return static_cast<UObject*>(nullptr);
		}();

		if(Object != nullptr)
		{
			check(Object->GetClass() == ObjectInformation.ObjectClass);

			ObjectMap.Emplace(ObjectInformation.ReadableObject, Object);
			ObjectReadableObjectMap.Emplace(Object, ObjectInformation.ReadableObject);
		}
	}

	for (const TPair<const UObject*, FLudeoReadableObject>& Pair : ObjectReadableObjectMap)
	{
		UObject* const& Object = const_cast<UObject*>(Pair.Get<0>());

		if(Object->GetOuter() == GetTransientPackage())
		{
			const FLudeoObjectHandle OuterLudeoObjectHandle = GetOuterObjectLudeoObjectHandle(Pair.Get<1>());
					
			const UObject* const* OuterObject =ObjectMap.FindByHash(GetTypeHash(OuterLudeoObjectHandle), OuterLudeoObjectHandle);
			check(OuterObject != nullptr);

			Object->Rename(nullptr, const_cast<UObject*>(*OuterObject));
		}
	}

	return ObjectMap;
}

TArray<const UObject*> FLudeoObjectStateManager::GetObjectsOfClass(const UWorld* World, const TSubclassOf<UObject>& ObjectClass)
{
	check(World != nullptr);

	TArray<const UObject*> ObjectCollection;

	if(ObjectClass->IsChildOf(AActor::StaticClass()))
	{
		for (TActorIterator<AActor> Itr(const_cast<UWorld*>(World), ObjectClass.Get()); Itr; ++Itr)
		{
			if (!Itr->IsPendingKillOrUnreachable() && (Itr->GetClass() == ObjectClass))
			{
				ObjectCollection.Add(*Itr);
			}
		}
	}
	else
	{
		for (FUnsafeObjectIterator Itr(ObjectClass); Itr; ++Itr)
		{
			if ((World == Itr->GetWorld()) && !Itr->IsPendingKillOrUnreachable() && (Itr->GetClass() == ObjectClass))
			{
				ObjectCollection.Add(*Itr);
			}
		}
	}

	return ObjectCollection;
}

bool FLudeoObjectStateManager::RestoreWorld
(
	const FLudeo& Ludeo,
	const UObject* WorldContextObject,
	const FLudeoSaveGameSpecification& SaveGameSpecification
)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	const FLudeoReadableObject::ReadableObjectMapType ObjectMap = FLudeoObjectStateManager::CreateRestoreWorldObjectMap
	(
		Ludeo,
		WorldContextObject,
		SaveGameSpecification
	);

	const auto RestoreAdditionalSaveGameProperty = [&](const FLudeoReadableObject& ReadableObject, const UObject* Object, const TArray<FName>& AdditionalSaveGamePropertyCollection)
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

	if (!bIsAllDataReadSuccessfully)
	{
		for (FLudeoReadableObject::ReadableObjectMapType::TConstIterator Itr = ObjectMap.CreateConstIterator(); Itr; ++Itr)
		{
			UObject* Object = const_cast<UObject*>(Itr->Get<1>());

			if (AActor* Actor = Cast<AActor>(Object))
			{
				World->DestroyActor(Actor, true);
			}
			else
			{
				Object->BeginDestroy();
			}
		}
	}

	return bIsAllDataReadSuccessfully;
}
