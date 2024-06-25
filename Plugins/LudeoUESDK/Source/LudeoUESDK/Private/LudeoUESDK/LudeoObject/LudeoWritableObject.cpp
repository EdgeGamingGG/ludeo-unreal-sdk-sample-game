#include "LudeoUESDK/LudeoObject/LudeoWritableObject.h"

#include "GameFramework/PlayerState.h"
#include "Engine/UserDefinedStruct.h"

#include <Ludeo/DataWriter.h>
#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoScopedGuard.h"
#include "LudeoUESDK/LudeoUtility.h"
#include "LudeoUESDK/LudeoProfiling.h"

// Suppress unreal wrong deprecated warning
#undef UInt16Property
#undef UInt32Property
#undef UInt64Property

DECLARE_CYCLE_STAT(TEXT("FLudeoWritableObject::WriteData (Object)"), STAT_WriteObject, STATGROUP_Ludeo);
DECLARE_CYCLE_STAT(TEXT("FLudeoWritableObject::BindPlayer"), STAT_BindPlayer, STATGROUP_Ludeo);
DECLARE_CYCLE_STAT(TEXT("FLudeoWritableObject::UnbindPlayer"), STAT_UnbindPlayer, STATGROUP_Ludeo);

FLudeoResult ConditionalRoomWriterSetCurrent(const FLudeoRoomWriterHandle& RoomWriterHandle)
{
	static FLudeoRoomWriterHandle CurrentRoomWriterHandle = nullptr;

	FLudeoResult Result(LudeoResult::Success);

	if (CurrentRoomWriterHandle != RoomWriterHandle)
	{
		Result = ludeo_DataWriter_SetCurrent(RoomWriterHandle);

		if (Result.IsSuccessful())
		{
			CurrentRoomWriterHandle = RoomWriterHandle;
		}
	}

	return Result;
}

template<typename DataType>
bool GenericWriteData(const FLudeoRoomWriterHandle& RoomWriterHandle, const char* AttributeName, const DataType& Data)
{
	check(AttributeName != nullptr);

	const FLudeoResult ConditionalSetCurrentResult = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (ConditionalSetCurrentResult.IsSuccessful())
	{
		return (ludeo_DataWriter_Set(AttributeName, Data) == LUDEO_TRUE);
	}

	return false;
}

FLudeoWritableObject::FLudeoWritableObject(const UObject* Object, const FLudeoObjectHandle& InObjectHandle, const FLudeoRoomWriterHandle& InRoomWriterHandle) :
	FLudeoObject(InObjectHandle),
	RoomWriterHandle(InRoomWriterHandle),
	WeakObject(Object)
{
	check(IsValid());
	check(GetObject() != nullptr);
	check(RoomWriterHandle != nullptr);
}

FLudeoWritableObject::~FLudeoWritableObject()
{

}

const UObject* FLudeoWritableObject::GetObject() const
{
	return WeakObject.Get();
}

bool FLudeoWritableObject::EnterObject() const
{
	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_EnterObject(static_cast<FLudeoObjectHandle>(*this)) == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoWritableObject::LeaveObject() const
{
	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_LeaveObject() == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoWritableObject::EnterComponent(const char* AttributeName) const
{
	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_EnterComponent(AttributeName) == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoWritableObject::LeaveComponent() const
{
	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_LeaveComponent() == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoWritableObject::BindPlayer(const char* PlayerID) const
{
	SCOPE_CYCLE_COUNTER(STAT_BindPlayer);

	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_SetPlayerBinding(PlayerID) == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoWritableObject::UnbindPlayer() const
{
	SCOPE_CYCLE_COUNTER(STAT_UnbindPlayer)

	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_SetPlayerBinding(nullptr) == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoWritableObject::WriteData
(
	const WritableObjectMapType& ObjectMap,
	const FLudeoObjectPropertyFilter& PropertyFilter
) const
{
	SCOPE_CYCLE_COUNTER(STAT_WriteObject);

	if(const UObject* Data = WeakObject.Get())
	{
		return InternalWriteData(Data->GetClass(), Data, ObjectMap, PropertyFilter);
	}
	
	return false;
}

bool FLudeoWritableObject::InternalWriteData
(
	const UStruct* StructureType,
	const void* StructureContainer,
	const WritableObjectMapType& ObjectMap,
	const FLudeoObjectPropertyFilter& PropertyFilter
) const
{
	check(StructureType != nullptr);
	check(StructureContainer != nullptr);

	bool bIsAllDataWrittenSuccessfully = true;

	for (TFieldIterator<FProperty> PropertyIterator(StructureType); bIsAllDataWrittenSuccessfully && PropertyIterator; ++PropertyIterator)
	{
		const FProperty* Property = *PropertyIterator;
		check(Property != nullptr);

		const bool bShouldWriteData = [&]()
		{
			if (PropertyFilter.Match(*Property))
			{
				bool bIsIdentical = true;

				if (const UScriptStruct* ScriptStruct = Cast<UScriptStruct>(StructureType))
				{
					char DefaultObjectBuffer[4096];

					check(ScriptStruct->GetStructureSize() <= sizeof(DefaultObjectBuffer));

					ScriptStruct->InitializeStruct(DefaultObjectBuffer);

					bIsIdentical = Property->Identical_InContainer(StructureContainer, DefaultObjectBuffer);

					ScriptStruct->DestroyStruct(DefaultObjectBuffer);
				}
				else if (const UClass* UnrealClass = Cast<UClass>(StructureType))
				{
					bIsIdentical = Property->Identical_InContainer(StructureContainer, UnrealClass->GetDefaultObject());
				}

				return !bIsIdentical;
			}

			return false;
		}();

		if (bShouldWriteData)
		{
			// Blueprint structure
			if (StructureType->IsA(UUserDefinedStruct::StaticClass()))
			{
				bIsAllDataWrittenSuccessfully = WriteData
				(
					LUDEO_FSTRING_TO_UTF8(StructureType->GetAuthoredNameForField(Property)),
					StructureContainer,
					Property,
					ObjectMap
				);
			}
			else
			{
				bIsAllDataWrittenSuccessfully = WriteData
				(
					LUDEO_FNAME_TO_UTF8(Property->GetFName()),
					StructureContainer,
					Property,
					ObjectMap
				);
			}
		}
	}

	return bIsAllDataWrittenSuccessfully;
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const int8& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const int16& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const int32& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const int64& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const uint8& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const uint16& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const uint32& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const uint64& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const bool& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const float& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const double& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FVector2D& Data) const
{
	return WriteData(AttributeName, FLudeoVector2D::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FVector& Data) const
{
	return WriteData(AttributeName, FLudeoVector::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FVector4& Data) const
{
	return WriteData(AttributeName, FLudeoVector4::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FRotator& Data) const
{
	return WriteData(AttributeName, FLudeoRotator::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FQuat& Data) const
{
	return WriteData(AttributeName, FLudeoQuaterion::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FTransform& Data) const
{
	return WriteData(AttributeName, FLudeoTransform::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const char* Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const TCHAR* Data) const
{
	const FTCHARToUTF8 StringConverter(Data);

	if(StringConverter.Length() > 0)
	{
		return WriteData(AttributeName, StringConverter.Get());
	}

	return WriteData(AttributeName, "");
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FString& Data) const
{
	return WriteData(AttributeName, *Data);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FName& Data) const
{
	return WriteData(AttributeName, LUDEO_FNAME_TO_UTF8(Data));
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FText& Data) const
{
	FLudeoText LudeoText;

	if (Data.IsCultureInvariant())
	{
		LudeoText.SourceString = Data.ToString();
	}
	else if (Data.IsFromStringTable())
	{
		FTextInspector::GetTableIdAndKey(Data, LudeoText.StringTableID, LudeoText.Key);	
	}
	else
	{
		// Save the namespace
		{
			TOptional<FString> Namespace = FTextInspector::GetNamespace(Data);

			if (Namespace.IsSet())
			{
				LudeoText.Namespace = MoveTemp(Namespace.GetValue());
			}
		}

		// Save the key
		{
			TOptional<FString> Key = FTextInspector::GetKey(Data);

			if(Key.IsSet())
			{
				LudeoText.Key = MoveTemp(Key.GetValue());
			}
		}

		if (const FString* SourceString = FTextInspector::GetSourceString(Data))
		{
			LudeoText.SourceString = *SourceString;
		}
	}

	return WriteData(AttributeName, FLudeoText::StaticStruct(), &LudeoText, {});
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, UClass* Data) const
{
	if (Data != nullptr)
	{
		#if WITH_EDITOR
			const FString ClassPathName = UWorld::RemovePIEPrefix(Data->GetPathName());
		#else
			const FString ClassPathName = Data->GetPathName();
		#endif

		return WriteData(AttributeName, *ClassPathName);
	}

	return WriteData(AttributeName, FString());
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FLudeoObjectHandle& Data) const
{
	return WriteData(AttributeName, static_cast<LudeoObjectId>(Data));
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const UObject* Data, const WritableObjectMapType& ObjectMap) const
{
	const FLudeoObjectHandle LudeoObjectHandle = [&]()
	{
		if (Data != nullptr && !Data->IsPendingKillOrUnreachable())
		{
			if (const FLudeoWritableObject* LudeoObject = ObjectMap.Find(Data))
			{
				return static_cast<FLudeoObjectHandle>(*LudeoObject);
			}
			else
			{
				check(false);
			}
		}

		return FLudeoObjectHandle(LUDEO_INVALID_OBJECTID);
	}();

	return WriteData(AttributeName, LudeoObjectHandle);
}

bool FLudeoWritableObject::WriteData(const char* AttributeName, const FWeakObjectPtr& Data, const WritableObjectMapType& ObjectMap) const
{
	return WriteData(AttributeName, Data.Get(), ObjectMap);
}

bool FLudeoWritableObject::WriteData
(
	const char* AttributeName,
	const UStruct* StructureType,
	const void* Structure,
	const WritableObjectMapType& ObjectMap,
	const FLudeoObjectPropertyFilter& PropertyFilter
) const
{
	const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject> EnterComponentGuard(*this, AttributeName);
	check(EnterComponentGuard.HasEnteredComponent());

	if(EnterComponentGuard.HasEnteredComponent())
	{
		return InternalWriteData(StructureType, Structure, ObjectMap, PropertyFilter);
	}

	return false;
}

bool FLudeoWritableObject::WriteData
(
	const char* AttributeName,
	const void* PropertyContainer,
	const FProperty* Property,
	const WritableObjectMapType& ObjectMap
) const
{
	check(PropertyContainer != nullptr);
	check(Property != nullptr);

	bool bIsDataWrittenSuccessfully = false;

	if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		FNumericProperty* UnderlyingProperty = EnumProperty->GetUnderlyingProperty();
		check(UnderlyingProperty != nullptr);

		bIsDataWrittenSuccessfully = WriteData
		(
			AttributeName,
			UnderlyingProperty->ContainerPtrToValuePtr<void>(PropertyContainer),
			UnderlyingProperty,
			ObjectMap
		);
	}
	else if (const FInt8Property* Int8Property = CastField<FInt8Property>(Property))
	{
		const int8& Data = Int8Property->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FInt16Property* Int16Property = CastField<FInt16Property>(Property))
	{
		const int16& Data = Int16Property->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FIntProperty* Int32Property = CastField<FIntProperty>(Property))
	{
		const int32& Data = Int32Property->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FInt64Property* Int64Property = CastField<FInt64Property>(Property))
	{
		const int64& Data = Int64Property->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
	{
		const uint8& Data = ByteProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FUInt16Property* UInt16Property = CastField<FUInt16Property>(Property))
	{
		const uint16& Data = UInt16Property->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FUInt32Property* UInt32Property = CastField<FUInt32Property>(Property))
	{
		const uint32& Data = UInt32Property->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FUInt64Property* UInt64Property = CastField<FUInt64Property>(Property))
	{
		const uint64& Data = UInt64Property->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		const bool Data = BoolProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		const float& Data = FloatProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
	{
		const double& Data = DoubleProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		const FString& Data = StringProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
	{
		const FName& Data = NameProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		const FText& Data = TextProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
	{
		UClass* Data = Cast<UClass>(ClassProperty->GetPropertyValue_InContainer(PropertyContainer));

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Data);
	}
	else if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		bIsDataWrittenSuccessfully = WriteData
		(
			AttributeName,
			StructProperty->Struct,
			StructProperty->ContainerPtrToValuePtr<void>(PropertyContainer),
			ObjectMap
		);
	}
	else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		const UObject* Object = ObjectProperty->GetObjectPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, Object, ObjectMap);
	}
	else if (const FWeakObjectProperty* WeakObjectProperty = CastField<FWeakObjectProperty>(Property))
	{
		const FWeakObjectPtr& WeakObjectPointer = WeakObjectProperty->GetPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, WeakObjectPointer, ObjectMap);
	}
	else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{	
		bIsDataWrittenSuccessfully = true;

		FScriptArrayHelper ScriptArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

		if(ScriptArrayHelper.Num() > 0)
		{
			const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject> EnterComponentGuard(*this, AttributeName);
			check(EnterComponentGuard.HasEnteredComponent());

			if(EnterComponentGuard.HasEnteredComponent())
			{
				bIsDataWrittenSuccessfully = WriteData("ArraySize", ScriptArrayHelper.Num());

				char ArrayElementAttributeNameBuffer[16];

				for (int32 i = 0; (bIsDataWrittenSuccessfully && i < ScriptArrayHelper.Num()); ++i)
				{
					FCStringAnsi::Snprintf(ArrayElementAttributeNameBuffer, sizeof(ArrayElementAttributeNameBuffer), "%d", i);

					bIsDataWrittenSuccessfully = WriteData
					(
						ArrayElementAttributeNameBuffer,
						ScriptArrayHelper.GetRawPtr(i),
						ArrayProperty->Inner,
						ObjectMap
					);
				}
			}
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		bIsDataWrittenSuccessfully = true;

		const FScriptSetHelper ScriptSetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

		if(ScriptSetHelper.Num() > 0)
		{
			const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject> EnterComponentGuard(*this, AttributeName);
			check(EnterComponentGuard.HasEnteredComponent());

			if (EnterComponentGuard.HasEnteredComponent())
			{
				bIsDataWrittenSuccessfully = WriteData("SetSize", ScriptSetHelper.Num());

				{
					char SetElementAttributeNameBuffer[16];

					int32 ElementNumber = 0;		

					for (FScriptSetHelper::FIterator Itr(ScriptSetHelper);  (bIsDataWrittenSuccessfully && Itr); ++Itr)
					{
						FCStringAnsi::Snprintf(SetElementAttributeNameBuffer, sizeof(SetElementAttributeNameBuffer), "%d", ElementNumber);

						bIsDataWrittenSuccessfully = WriteData
						(
							SetElementAttributeNameBuffer,
							ScriptSetHelper.GetElementPtr(*Itr),
							ScriptSetHelper.GetElementProperty(),
							ObjectMap
						);

						ElementNumber = ElementNumber + 1;
					}
				}
			}
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		bIsDataWrittenSuccessfully = true;

		FScriptMapHelper ScriptMapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

		if(ScriptMapHelper.Num() > 0)
		{
			const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject> EnterComponentGuard(*this, AttributeName);
			check(EnterComponentGuard.HasEnteredComponent());

			if (EnterComponentGuard.HasEnteredComponent())
			{
				bIsDataWrittenSuccessfully = WriteData("MapSize", ScriptMapHelper.Num());

				{
					char MapPairAttributeNameBuffer[32];

					int32 PairNumber = 0;

					for (FScriptMapHelper::FIterator Itr(ScriptMapHelper); (bIsDataWrittenSuccessfully && Itr); ++Itr)
					{
						FCStringAnsi::Snprintf(MapPairAttributeNameBuffer, sizeof(MapPairAttributeNameBuffer), "Key_%d", PairNumber),
						bIsDataWrittenSuccessfully = WriteData
						(
							
							MapPairAttributeNameBuffer,
							ScriptMapHelper.GetKeyPtr(*Itr),
							ScriptMapHelper.GetKeyProperty(),
							ObjectMap
						);

						FCStringAnsi::Snprintf(MapPairAttributeNameBuffer, sizeof(MapPairAttributeNameBuffer), "Value_%d", PairNumber),
						bIsDataWrittenSuccessfully = bIsDataWrittenSuccessfully && WriteData
						(
							MapPairAttributeNameBuffer,
							ScriptMapHelper.GetValuePtr(*Itr),
							ScriptMapHelper.GetKeyProperty(),
							ObjectMap
						);

						PairNumber = PairNumber + 1;
					}
				}
			}
		}
	}
	else
	{
		ensure(false);
	}

	return bIsDataWrittenSuccessfully;
}

