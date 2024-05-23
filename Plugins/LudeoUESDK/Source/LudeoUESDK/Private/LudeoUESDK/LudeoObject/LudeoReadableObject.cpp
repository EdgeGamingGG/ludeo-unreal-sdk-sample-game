#include "LudeoUESDK/LudeoObject/LudeoReadableObject.h"

#include <Ludeo/DataReader.h>

#include "LudeoUESDK/LudeoScopedGuard.h"
#include "LudeoUESDK/LudeoLog/LudeoLogTypes.h"
#include "LudeoUESDK/LudeoResult.h"

// Suppress unreal wrong deprecated warning
#undef UInt16Property
#undef UInt32Property
#undef UInt64Property

FORCEINLINE uint32 GetTypeHash(const FLudeoReadableObject& ReadableObject)
{
	return GetTypeHash(static_cast<FLudeoObjectHandle>(ReadableObject));
}

FLudeoResult ConditionalDataReaderSetCurrent(const FLudeoHandle& LudeoHandle)
{
	static FLudeoHandle CurrentLudeoHandle = nullptr;

	FLudeoResult Result(LudeoResult::Success);

	if (CurrentLudeoHandle != LudeoHandle)
	{
		Result = ludeo_DataReader_SetCurrent(LudeoHandle);

		if (Result.IsSuccessful())
		{
			CurrentLudeoHandle = LudeoHandle;
		}
	}

	return Result;
}

template<typename DataType>
bool GenericReadData(const FLudeoHandle& LudeoHandle, const char* AttributeName, DataType& Data)
{
	check(AttributeName != nullptr);

	const FLudeoResult ConditionalSetCurrentResult = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (ConditionalSetCurrentResult.IsSuccessful())
	{
		return (ludeo_DataReader_Get(AttributeName, Data) == LUDEO_TRUE);
	}

	return false;
}

bool GenericReadData(const FLudeoHandle& LudeoHandle, const char* AttributeName, FString& Data)
{
	check(AttributeName != nullptr);

	const FLudeoResult ConditionalSetCurrentResult = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (ConditionalSetCurrentResult.IsSuccessful())
	{
		uint32_t BufferSize = 0;
		const bool bHasGetSize = (ludeo_DataReader_GetSize(AttributeName, &BufferSize) == LUDEO_TRUE);
		check(bHasGetSize);

		check(BufferSize <= static_cast<uint32>(TNumericLimits<int32>::Max()));

		bool bHasReadData = true;

		if (BufferSize > 0)
		{
			TArray<ANSICHAR> Buffer;
			Buffer.AddUninitialized(FMath::Max(1, static_cast<int32>(BufferSize / sizeof(ANSICHAR))));

			char* BufferData = Buffer.GetData();

			bHasReadData = GenericReadData(LudeoHandle, AttributeName, BufferData);
			check(bHasReadData);

			Data = UTF8_TO_TCHAR(BufferData);
		}

		return bHasReadData;
	}

	return false;
}

FLudeoReadableObject::FLudeoReadableObject(const FLudeoObjectHandle InObjectHandle, const FLudeoHandle& InLudeoHandle) :
	FLudeoObject(InObjectHandle),
	LudeoHandle(InLudeoHandle)
{
	check(IsValid());
	check(LudeoHandle != nullptr);
}

FLudeoReadableObject::~FLudeoReadableObject()
{

}

bool FLudeoReadableObject::EnterObject() const
{
	const FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataReader_EnterObject(static_cast<FLudeoObjectHandle>(*this)) == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoReadableObject::LeaveObject() const
{
	const FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataReader_LeaveObject() == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoReadableObject::EnterComponent(const TCHAR* AttributeName) const
{
	const FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataReader_EnterComponent(TCHAR_TO_UTF8(AttributeName)) == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoReadableObject::LeaveComponent() const
{
	const FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataReader_LeaveComponent() == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoReadableObject::ExistAttribute(const TCHAR* AttributeName) const
{
	bool bAttributeExists = false;

	const FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		uint32_t DataByteSize = 0;

		bAttributeExists = (ludeo_DataReader_GetSize(TCHAR_TO_UTF8(AttributeName), &DataByteSize) == LUDEO_TRUE);

		if(!bAttributeExists)
		{
			const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject, true> EnterComponentGuard(*this, AttributeName);

			bAttributeExists = EnterComponentGuard.HasEnteredComponent();
		}
	}

	return bAttributeExists;
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, int8& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, int16& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, int32& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, int64& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, uint8& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, uint16& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, uint32& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, uint64& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, bool& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, float& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, double& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FVector2D& Data) const
{
	return ReadData(AttributeName, FLudeoVector2D::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FVector& Data) const
{
	return ReadData(AttributeName, FLudeoVector::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FVector4& Data) const
{
	return ReadData(AttributeName, FLudeoVector4::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FRotator& Data) const
{
	return ReadData(AttributeName, FLudeoRotator::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FQuat& Data) const
{
	return ReadData(AttributeName, FLudeoQuaterion::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FTransform& Data) const
{
	return ReadData(AttributeName, FLudeoTransform::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FString& Data) const
{
	return GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), Data);
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FName& Data) const
{
	FString NameString;

	const bool bHasReadData = GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), NameString);

	if (bHasReadData)
	{
		Data = FName(*NameString);
	}

	return bHasReadData;
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FText& Data) const
{
	FLudeoText LudeoText;

	const bool bHasReadData = ReadData(AttributeName, FLudeoText::StaticStruct(), &LudeoText, {});

	if (bHasReadData)
	{
		if (!LudeoText.StringTableID.IsNone() && !LudeoText.Key.IsEmpty())
		{
			Data = FText::FromStringTable(LudeoText.StringTableID, LudeoText.Key);
		}
		else if(!LudeoText.Namespace.IsEmpty() && !LudeoText.Key.IsEmpty())
		{
			Data = FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText(*LudeoText.Namespace, *LudeoText.Key, *LudeoText.SourceString);
		}
		else
		{
			Data = FText::FromString(LudeoText.SourceString);
		}
	}

	return bHasReadData;
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, UClass*& Data, FString* ClassPathName) const
{
	FString StringData;

	if (GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), StringData))
	{
		if (StringData.Len() > 0)
		{
			if (UClass* LoadedClass = LoadClass<UObject>(nullptr, *StringData))
			{
				Data = LoadedClass;
			}
			else
			{
				UE_LOG(LogLudeo, Warning, TEXT(R"(Attribute "%s" was read succesfully but class cannot be loaded with the associated string data)"), *StringData);
			}
		}
		else
		{
			Data = nullptr;
		}

		if (ClassPathName != nullptr)
		{
			*ClassPathName = MoveTemp(StringData);
		}

		return true;
	}

	return false;
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FLudeoObjectHandle& Data) const
{
	LudeoObjectId ObjectID = LUDEO_INVALID_OBJECTID;
	
	if (GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), ObjectID))
	{
		Data = FLudeoObjectHandle(ObjectID);
		
		return true;
	}

	return false;
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, UObject*& Data, const ReadableObjectMapType& ObjectMap) const
{
	LudeoObjectId ObjectID = LUDEO_INVALID_OBJECTID;

	if (GenericReadData(LudeoHandle, TCHAR_TO_UTF8(AttributeName), ObjectID))
	{
		const FLudeoObjectHandle LudeoObjectHandle(ObjectID);

		if (LudeoObjectHandle == LUDEO_INVALID_OBJECTID)
		{
			Data = nullptr;
		}
		else
		{
			if (UObject* const* pObject = ObjectMap.FindByHash(GetTypeHash(LudeoObjectHandle), LudeoObjectHandle))
			{
				Data = *pObject;
			}
			else
			{
				check(false);
			}
		}

		return true;
	}

	return false;
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, FWeakObjectPtr& Data, const ReadableObjectMapType& ObjectMap) const
{
	UObject* Object = nullptr;

	if (ReadData(AttributeName, Object, ObjectMap))
	{
		Data = FWeakObjectPtr(Object);

		return true;
	}

	return false;
}

bool FLudeoReadableObject::ReadData
(
	const TCHAR* AttributeName,
	const UStruct* StructureType,
	void* Structure,
	const ReadableObjectMapType& ObjectMap,
	const FLudeoObjectPropertyFilter& PropertyFilter
) const
{
	const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject, true> EnterComponentGuard
	(
		*this,
		AttributeName
	);

	if(EnterComponentGuard.HasEnteredComponent())
	{
		return InternalReadData(StructureType, Structure, ObjectMap, PropertyFilter);
	}

	return false;
}

bool FLudeoReadableObject::ReadData(const TCHAR* AttributeName, const void* PropertyContainer, const FProperty* Property, const ReadableObjectMapType& ObjectMap) const
{
	check(PropertyContainer != nullptr);
	check(Property != nullptr);

	bool bIsDataReadSuccessfully = false;

	if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		bIsDataReadSuccessfully = ReadData
		(
			AttributeName,
			EnumProperty->ContainerPtrToValuePtr<void>(PropertyContainer),
			EnumProperty->GetUnderlyingProperty(),
			ObjectMap
		);
	}
	else if (const FInt8Property* Int8Property = CastField<FInt8Property>(Property))
	{
		int8& Data = *Int8Property->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FInt16Property* Int16Property = CastField<FInt16Property>(Property))
	{
		int16& Data = *Int16Property->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FIntProperty* Int32Property = CastField<FIntProperty>(Property))
	{
		int32& Data = *Int32Property->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FInt64Property* Int64Property = CastField<FInt64Property>(Property))
	{
		int64& Data = *Int64Property->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
	{
		uint8& Data = *ByteProperty->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FUInt16Property* UInt16Property = CastField<FUInt16Property>(Property))
	{
		uint16& Data = *UInt16Property->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FUInt32Property* UInt32Property = CastField<FUInt32Property>(Property))
	{
		uint32& Data = *UInt32Property->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FUInt64Property* UInt64Property = CastField<FUInt64Property>(Property))
	{
		uint64& Data = *UInt64Property->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		bool Data = false;

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);

		if(bIsDataReadSuccessfully)
		{
			BoolProperty->SetPropertyValue_InContainer(const_cast<void*>(PropertyContainer), Data);
		}
	}
	else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		float& Data = *FloatProperty->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
	{
		double& Data = *DoubleProperty->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		FString& Data = *StringProperty->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
	{
		FName& Data = *NameProperty->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		FText& Data = *TextProperty->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Data);
	}
	else if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
	{
		UClass* ObjectClass = nullptr;

		bIsDataReadSuccessfully = ReadData(AttributeName, ObjectClass);

		if (bIsDataReadSuccessfully)
		{
			ClassProperty->SetObjectPropertyValue_InContainer(const_cast<void*>(PropertyContainer), ObjectClass);
		}
	}
	else if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		bIsDataReadSuccessfully = ReadData
		(
			AttributeName,
			StructProperty->Struct,
			StructProperty->ContainerPtrToValuePtr<void>(const_cast<void*>(PropertyContainer)),
			ObjectMap
		);
	}
	else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		UObject*& Object = const_cast<UObject*&>(*ObjectProperty->GetPropertyValuePtr_InContainer(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, Object, ObjectMap);
	}
	else if (const FWeakObjectProperty* WeakObjectProperty = CastField<FWeakObjectProperty>(Property))
	{
		FWeakObjectPtr& WeakObjectPointer = *WeakObjectProperty->GetPropertyValuePtr_InContainer(const_cast<void*>(PropertyContainer));

		bIsDataReadSuccessfully = ReadData(AttributeName, WeakObjectPointer, ObjectMap);
	}
	else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject, true> EnterComponentGuard(*this, AttributeName);

		bIsDataReadSuccessfully = EnterComponentGuard.HasEnteredComponent();

		if(bIsDataReadSuccessfully)
		{
			int32 ArraySize = 0;
			ReadData(TEXT("ArraySize"), ArraySize);

			if(ArraySize >= 0)
			{
				FScriptArrayHelper ScriptArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

				ScriptArrayHelper.EmptyValues(ArraySize);

				for (int32 i = 0; (bIsDataReadSuccessfully && i < ArraySize); ++i)
				{
					const int32 Index = ScriptArrayHelper.AddValue();

					bIsDataReadSuccessfully = ReadData
					(
						*FString::FromInt(i),
						ScriptArrayHelper.GetRawPtr(Index),
						ArrayProperty->Inner,
						ObjectMap
					);
				}
			}
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject, true> EnterComponentGuard(*this, AttributeName);

		bIsDataReadSuccessfully = EnterComponentGuard.HasEnteredComponent();

		if (bIsDataReadSuccessfully)
		{
			int32 SetSize = 0;		
			ReadData(TEXT("SetSize"), SetSize);

			if(SetSize >= 0)
			{
				FScriptSetHelper ScriptSetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

				ScriptSetHelper.EmptyElements(SetSize);

				for (int32 i = 0; (bIsDataReadSuccessfully && i < SetSize); ++i)
				{
					const int32 Index = ScriptSetHelper.AddDefaultValue_Invalid_NeedsRehash();

					bIsDataReadSuccessfully = ReadData
					(
						*FString::FromInt(i),
						ScriptSetHelper.GetElementPtr(Index),
						ScriptSetHelper.GetElementProperty(),
						ObjectMap
					);
				}

				ScriptSetHelper.Rehash();
			}
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject, true> EnterComponentGuard(*this, AttributeName);

		bIsDataReadSuccessfully = EnterComponentGuard.HasEnteredComponent();

		if (bIsDataReadSuccessfully)
		{
			int32 MapSize = 0;
			ReadData(TEXT("MapSize"), MapSize);

			if(MapSize >= 0)
			{
				FScriptMapHelper ScriptMapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

				ScriptMapHelper.EmptyValues(MapSize);

				for (int32 i = 0; (bIsDataReadSuccessfully && i < MapSize); ++i)
				{
					const int32 Index = ScriptMapHelper.AddDefaultValue_Invalid_NeedsRehash();

					bIsDataReadSuccessfully = bIsDataReadSuccessfully && ReadData
					(
						*FString::Printf(TEXT("Key_%d"), i),
						ScriptMapHelper.GetKeyPtr(Index),
						ScriptMapHelper.GetKeyProperty(),
						ObjectMap
					);

					bIsDataReadSuccessfully = bIsDataReadSuccessfully && ReadData
					(
						*FString::Printf(TEXT("Value_%d"), i),
						ScriptMapHelper.GetValuePtr(Index),
						ScriptMapHelper.GetValueProperty(),
						ObjectMap
					);
				}

				ScriptMapHelper.Rehash();
			}
		}
	}
	else
	{
		ensure(false);
	}

	return bIsDataReadSuccessfully;
}

bool FLudeoReadableObject::ReadData
(
	const UObject* Data,
	const ReadableObjectMapType& ObjectMap,
	const FLudeoObjectPropertyFilter& PropertyFilter
) const
{
	check(Data != nullptr);

	return InternalReadData(Data->GetClass(), const_cast<UObject*>(Data), ObjectMap, PropertyFilter);
}

bool FLudeoReadableObject::InternalReadData
(
	const UStruct* StructureType,
	void* StructureContainer,
	const ReadableObjectMapType& ObjectMap,
	const FLudeoObjectPropertyFilter& PropertyFilter
) const
{
	bool bIsAllDataReadSuccessfully = true;

	for (TFieldIterator<FProperty> PropertyIterator(StructureType); (bIsAllDataReadSuccessfully && PropertyIterator); ++PropertyIterator)
	{
		const FProperty* Property = *PropertyIterator;
		check(Property != nullptr);

		if (PropertyFilter.Match(*Property))
		{
			const FString PropertyName = Property->GetName();

			bIsAllDataReadSuccessfully = !ExistAttribute(*PropertyName) || ReadData(*PropertyName, StructureContainer, Property, ObjectMap);
		}
	}

	return bIsAllDataReadSuccessfully;
}
