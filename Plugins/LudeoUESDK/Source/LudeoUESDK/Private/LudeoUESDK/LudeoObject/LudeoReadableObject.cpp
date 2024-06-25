#include "LudeoUESDK/LudeoObject/LudeoReadableObject.h"

#include <Ludeo/DataReader.h>

#include "LudeoUESDK/LudeoScopedGuard.h"
#include "LudeoUESDK/LudeoLog/LudeoLogTypes.h"
#include "LudeoUESDK/LudeoResult.h"
#include "LudeoUESDK/LudeoUtility.h"
#include "LudeoUESDK/LudeoProfiling.h"

// Suppress unreal wrong deprecated warning
#undef UInt16Property
#undef UInt32Property
#undef UInt64Property

DECLARE_CYCLE_STAT(TEXT("FLudeoWritableObject::ReadData (Object)"), STAT_ReadObject, STATGROUP_Ludeo);

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
		uint32_t StringBufferSize = 0;
		const bool bHasGetSize = (ludeo_DataReader_GetSize(AttributeName, &StringBufferSize) == LUDEO_TRUE);
		check(bHasGetSize);
		check(StringBufferSize <= static_cast<uint32>(TNumericLimits<int32>::Max()));

		if(bHasGetSize)
		{
			if (StringBufferSize > 0)
			{
				TArray<char> StringBuffer;
				StringBuffer.AddUninitialized(StringBufferSize);

				char* StringBufferData = StringBuffer.GetData();

				const bool bHasReadData = GenericReadData(LudeoHandle, AttributeName, StringBufferData);
				check(bHasReadData);

				if(bHasReadData)
				{
					const FUTF8ToTCHAR StringConverter(StringBufferData, StringBufferSize);
					Data = FString(StringConverter.Length(), StringConverter.Get());

					return true;
				}
			}
			else
			{
				return true;
			}
		}
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

bool FLudeoReadableObject::EnterComponent(const char* AttributeName) const
{
	const FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataReader_EnterComponent(AttributeName) == LUDEO_TRUE);
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

TOptionalWithLudeoResult<LudeoDataType> FLudeoReadableObject::GetAttributeDataType(const char* AttributeName) const
{
	LudeoDataType DataType = static_cast<LudeoDataType>(TNumericLimits<int32>::Max());

	FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		const bool bAttributeExists = (ludeo_DataReader_AttributeExists(AttributeName, &DataType) == LUDEO_TRUE);

		if(!bAttributeExists)
		{
			Result = LudeoResult::NotFound;
		}
	}

	return TOptionalWithLudeoResult<LudeoDataType>(Result, DataType);
}

bool FLudeoReadableObject::ExistAttribute(const char* AttributeName) const
{
	return GetAttributeDataType(AttributeName).IsSuccessful();
}

TOptionalWithLudeoResult<uint32> FLudeoReadableObject::Iterate(const IteratePredicateType& Predicate) const
{
	struct VisitAttributesUserCallbackData
	{
		VisitAttributesUserCallbackData(const IteratePredicateType& Predicate) :
			Predicate(Predicate), NumberOfAttribute(0)
		{

		}

		const IteratePredicateType& Predicate;
		uint32 NumberOfAttribute;

	} UserCallbackData(Predicate);

	FLudeoResult Result = ConditionalDataReaderSetCurrent(LudeoHandle);

	if (Result.IsSuccessful())
	{
		const LudeoDataReaderVisitAttributesParams VisitAttributesParams = Ludeo::create<LudeoDataReaderVisitAttributesParams>();

		if(UserCallbackData.Predicate)
		{
			Result = ludeo_DataReader_VisitAttributes
			(
				&VisitAttributesParams,
				&UserCallbackData.NumberOfAttribute,
				&UserCallbackData,
				[](const LudeoDataReaderVisitAttributesCallbackParams* CallbackParameter)
				{
					check(CallbackParameter != nullptr);

					if (CallbackParameter != nullptr)
					{
						if (VisitAttributesUserCallbackData* pUserCallbackData = static_cast<VisitAttributesUserCallbackData*>(CallbackParameter->clientData))
						{
							VisitAttributesUserCallbackData& UserCallbackData = *pUserCallbackData;

							if (UserCallbackData.Predicate)
							{
								const bool bContinue = UserCallbackData.Predicate
								(
									UserCallbackData.NumberOfAttribute,
									CallbackParameter->name,
									CallbackParameter->type
								);

								return (bContinue ? LUDEO_TRUE : LUDEO_FALSE);
							}
						}
					}

					return LUDEO_FALSE;
				}
			);
		}
		else
		{
			Result = ludeo_DataReader_VisitAttributes
			(
				&VisitAttributesParams,
				&UserCallbackData.NumberOfAttribute,
				nullptr,
				nullptr
			);
		}
	}

	return TOptionalWithLudeoResult<uint32>(Result, UserCallbackData.NumberOfAttribute);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, int8& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, int16& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, int32& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, int64& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, uint8& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, uint16& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, uint32& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, uint64& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, bool& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, float& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, double& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FVector2D& Data) const
{
	return ReadData(AttributeName, FLudeoVector2D::StaticStruct()->GetSuperStruct(), &Data, {}, {});
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FVector& Data) const
{
	return ReadData(AttributeName, FLudeoVector::StaticStruct()->GetSuperStruct(), &Data, {}, {});
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FVector4& Data) const
{
	return ReadData(AttributeName, FLudeoVector4::StaticStruct()->GetSuperStruct(), &Data, {}, {});
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FRotator& Data) const
{
	return ReadData(AttributeName, FLudeoRotator::StaticStruct()->GetSuperStruct(), &Data, {}, {});
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FQuat& Data) const
{
	return ReadData(AttributeName, FLudeoQuaterion::StaticStruct()->GetSuperStruct(), &Data, {}, {});
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FTransform& Data) const
{
	return ReadData(AttributeName, FLudeoTransform::StaticStruct()->GetSuperStruct(), &Data, {}, {});
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FString& Data) const
{
	return GenericReadData(LudeoHandle, AttributeName, Data);
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FName& Data) const
{
	FString NameString;

	const bool bHasReadData = GenericReadData(LudeoHandle, AttributeName, NameString);

	if (bHasReadData)
	{
		Data = FName(*NameString, NameString.Len());
	}

	return bHasReadData;
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, FText& Data) const
{
	FLudeoText LudeoText;

	const bool bHasReadData = ReadData(AttributeName, FLudeoText::StaticStruct(), &LudeoText, {}, {});

	if (bHasReadData)
	{
		if (!LudeoText.StringTableID.IsNone() && !LudeoText.Key.IsEmpty())
		{
			Data = FText::FromStringTable(LudeoText.StringTableID, LudeoText.Key);
		}
		else if(!LudeoText.Namespace.IsEmpty() && !LudeoText.Key.IsEmpty())
		{
			Data = FInternationalization::ForUseOnlyByLocMacroAndGraphNodeTextLiterals_CreateText
			(
				*LudeoText.Namespace,
				*LudeoText.Key,
				*LudeoText.SourceString
			);
		}
		else
		{
			Data = FText::FromString(LudeoText.SourceString);
		}
	}

	return bHasReadData;
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, UClass*& Data, FString* ClassPathName) const
{
	FString StringData;

	if (GenericReadData(LudeoHandle, AttributeName, StringData))
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

bool FLudeoReadableObject::ReadData(const char* AttributeName, FLudeoObjectHandle& Data) const
{
	LudeoObjectId ObjectID = LUDEO_INVALID_OBJECTID;
	
	if (GenericReadData(LudeoHandle, AttributeName, ObjectID))
	{
		Data = FLudeoObjectHandle(ObjectID);
		
		return true;
	}

	return false;
}

bool FLudeoReadableObject::ReadData(const char* AttributeName, UObject*& Data, const ReadableObjectMapType& ObjectMap) const
{
	LudeoObjectId ObjectID = LUDEO_INVALID_OBJECTID;

	if (GenericReadData(LudeoHandle, AttributeName, ObjectID))
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

bool FLudeoReadableObject::ReadData(const char* AttributeName, FWeakObjectPtr& Data, const ReadableObjectMapType& ObjectMap) const
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
	const char* AttributeName,
	const UStruct* StructureType,
	void* Structure,
	const ReadableObjectMapType& ObjectMap,
	const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
) const
{
	const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject> EnterComponentGuard
	(
		*this,
		AttributeName
	);
	check(EnterComponentGuard.HasEnteredComponent());

	if(EnterComponentGuard.HasEnteredComponent())
	{
		return InternalReadData(StructureType, Structure, ObjectMap, PropertyFilter);
	}

	return false;
}

bool FLudeoReadableObject::ReadData
(
	const char* AttributeName,
	const void* PropertyContainer,
	const FProperty* Property,
	const ReadableObjectMapType& ObjectMap,
	const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
) const
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
			ObjectMap,
			PropertyFilter
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
			ObjectMap,
			PropertyFilter
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
		const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject> EnterComponentGuard(*this, AttributeName);
		check(EnterComponentGuard.HasEnteredComponent());

		if(EnterComponentGuard.HasEnteredComponent())
		{
			bIsDataReadSuccessfully = true;

			int32 ArraySize = 0;
			ReadData("ArraySize", ArraySize);

			if(ArraySize >= 0)
			{
				char Buffer[16];

				FScriptArrayHelper ScriptArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

				ScriptArrayHelper.EmptyValues(ArraySize);

				for (int32 i = 0; (bIsDataReadSuccessfully && i < ArraySize); ++i)
				{
					const int32 Index = ScriptArrayHelper.AddValue();

					FCStringAnsi::Snprintf(Buffer, sizeof(Buffer), "%d", Index);

					bIsDataReadSuccessfully = ReadData
					(
						Buffer,
						ScriptArrayHelper.GetRawPtr(Index),
						ArrayProperty->Inner,
						ObjectMap,
						PropertyFilter
					);
				}
			}
		}
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject> EnterComponentGuard(*this, AttributeName);
		check(EnterComponentGuard.HasEnteredComponent());

		if (EnterComponentGuard.HasEnteredComponent())
		{
			bIsDataReadSuccessfully = true;

			int32 SetSize = 0;		
			ReadData("SetSize", SetSize);

			if(SetSize >= 0)
			{
				char Buffer[16];

				FScriptSetHelper ScriptSetHelper(SetProperty, SetProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

				ScriptSetHelper.EmptyElements(SetSize);

				for (int32 i = 0; (bIsDataReadSuccessfully && i < SetSize); ++i)
				{
					const int32 Index = ScriptSetHelper.AddDefaultValue_Invalid_NeedsRehash();

					FCStringAnsi::Snprintf(Buffer, sizeof(Buffer), "%d", Index);

					bIsDataReadSuccessfully = ReadData
					(
						Buffer,
						ScriptSetHelper.GetElementPtr(Index),
						ScriptSetHelper.GetElementProperty(),
						ObjectMap,
						PropertyFilter
					);
				}

				ScriptSetHelper.Rehash();
			}
		}
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoReadableObject> EnterComponentGuard(*this, AttributeName);
		check(EnterComponentGuard.HasEnteredComponent());

		if (EnterComponentGuard.HasEnteredComponent())
		{
			int32 MapSize = 0;
			ReadData("MapSize", MapSize);

			if(MapSize >= 0)
			{
				char Buffer[32];

				FScriptMapHelper ScriptMapHelper(MapProperty, MapProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

				ScriptMapHelper.EmptyValues(MapSize);

				for (int32 i = 0; (bIsDataReadSuccessfully && i < MapSize); ++i)
				{
					const int32 Index = ScriptMapHelper.AddDefaultValue_Invalid_NeedsRehash();

					FCStringAnsi::Snprintf(Buffer, sizeof(Buffer), "Key_%d", Index),
					bIsDataReadSuccessfully = bIsDataReadSuccessfully && ReadData
					(
						Buffer,
						ScriptMapHelper.GetKeyPtr(Index),
						ScriptMapHelper.GetKeyProperty(),
						ObjectMap,
						PropertyFilter
					);

					FCStringAnsi::Snprintf(Buffer, sizeof(Buffer), "Value_%d", Index),
					bIsDataReadSuccessfully = bIsDataReadSuccessfully && ReadData
					(
						Buffer,
						ScriptMapHelper.GetValuePtr(Index),
						ScriptMapHelper.GetValueProperty(),
						ObjectMap,
						PropertyFilter
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
	const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
) const
{
	SCOPE_CYCLE_COUNTER(STAT_ReadObject);

	if(Data != nullptr)
	{
		const FScopedLudeoDataReadWriteEnterObjectGuard<FLudeoReadableObject> EnterObjectGuard(*this);
		check(EnterObjectGuard.HasEnteredObject());

		if(EnterObjectGuard.HasEnteredObject())
		{
			return InternalReadData(Data->GetClass(), const_cast<UObject*>(Data), ObjectMap, PropertyFilter);
		}
	}

	return false;
}

bool FLudeoReadableObject::InternalReadData
(
	const UStruct* StructureType,
	void* StructureContainer,
	const ReadableObjectMapType& ObjectMap,
	const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
) const
{
	const auto GetStructureTypeName = [&]()
	{
		#if WITH_EDITOR
			return UWorld::RemovePIEPrefix(StructureType->GetPathName());
		#else
			return StructureType->GetPathName();
		#endif
	};

#if !UE_BUILD_SHIPPING
	for (TFieldIterator<FProperty> PropertyIterator(StructureType); PropertyIterator; ++PropertyIterator)
	{
		const FProperty* Property = *PropertyIterator;
		check(Property != nullptr);

		if (!PropertyFilter.IsSet() || PropertyFilter.GetValue().Match(*Property))
		{
			const FString PropertyName = StructureType->GetAuthoredNameForField(Property);

			if (!ExistAttribute(LUDEO_FSTRING_TO_UTF8(PropertyName)))
			{
				UE_LOG
				(
					LogLudeo,
					Log,
					TEXT(R"(Property "%s" matches the property filter but does not exist in the Ludeo data in the structure %s)"),
					*PropertyName,
					*GetStructureTypeName()
				);
			}
		}
	}
#endif

	bool bIsAllDataReadSuccessfully = true;

	const TOptionalWithLudeoResult<uint32> Result = Iterate([&](const uint32 NumberOfAttribute, const char* AttributeName, const LudeoDataType DataType)
	{
		if (const FProperty* Property = StructureType->FindPropertyByName(FName(UTF8_TO_TCHAR(AttributeName))))
		{
			if (IsPropertyMatchingDataType(Property, DataType))
			{
				if (!PropertyFilter.IsSet() || PropertyFilter.GetValue().Match(*Property))
				{
					bIsAllDataReadSuccessfully = ReadData(AttributeName, StructureContainer, Property, ObjectMap, PropertyFilter) && bIsAllDataReadSuccessfully;
				}
				else
				{
					UE_LOG
					(
						LogLudeo,
						Log,
						TEXT(R"(Attribute "%s" exists in Ludeo but does not match the property filter)"),
						UTF8_TO_TCHAR(AttributeName)
					);
				}	
			}
			else
			{
				ensure(false);

				UE_LOG
				(
					LogLudeo,
					Warning,
					TEXT(R"(Attribute "%s" exists in Ludeo but does not match the type "%s" found in the structure %s)"),
					UTF8_TO_TCHAR(AttributeName),
					*Property->GetCPPType(),
					*GetStructureTypeName()
				);
			}
		}
		else
		{
			UE_LOG
			(
				LogLudeo,
				Log,
				TEXT(R"(Attribute "%s" exists in Ludeo but does not find in the structure %s)"),
				UTF8_TO_TCHAR(AttributeName),
				*GetStructureTypeName()
			);
		}

		return true;
	});

	return (Result.IsSuccessful() && bIsAllDataReadSuccessfully);
}

bool FLudeoReadableObject::IsPropertyMatchingDataType(const FProperty* Property, const LudeoDataType DataType) const
{
	if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
	{
		return IsPropertyMatchingDataType(EnumProperty->GetUnderlyingProperty(), DataType);
	}
	else if (const FInt8Property* Int8Property = CastField<FInt8Property>(Property))
	{
		return (DataType == LudeoDataType::Int8);
	}
	else if (const FInt16Property* Int16Property = CastField<FInt16Property>(Property))
	{
		return (DataType == LudeoDataType::Int16);
	}
	else if (const FIntProperty* Int32Property = CastField<FIntProperty>(Property))
	{
		return (DataType == LudeoDataType::Int32);
	}
	else if (const FInt64Property* Int64Property = CastField<FInt64Property>(Property))
	{
		return (DataType == LudeoDataType::Int64);
	}
	else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
	{
		return (DataType == LudeoDataType::UInt8);
	}
	else if (const FUInt16Property* UInt16Property = CastField<FUInt16Property>(Property))
	{
		return (DataType == LudeoDataType::UInt16);
	}
	else if (const FUInt32Property* UInt32Property = CastField<FUInt32Property>(Property))
	{
		return (DataType == LudeoDataType::UInt32);
	}
	else if (const FUInt64Property* UInt64Property = CastField<FUInt64Property>(Property))
	{
		return (DataType == LudeoDataType::UInt64);
	}
	else if (const FBoolProperty* BoolProperty = CastField<FBoolProperty>(Property))
	{
		return (DataType == LudeoDataType::Bool);
	}
	else if (const FFloatProperty* FloatProperty = CastField<FFloatProperty>(Property))
	{
		return (DataType == LudeoDataType::Float);
	}
	else if (const FDoubleProperty* DoubleProperty = CastField<FDoubleProperty>(Property))
	{
		return (DataType == LudeoDataType::Double);
	}
	else if (const FStrProperty* StringProperty = CastField<FStrProperty>(Property))
	{
		return (DataType == LudeoDataType::String);
	}
	else if (const FNameProperty* NameProperty = CastField<FNameProperty>(Property))
	{
		return (DataType == LudeoDataType::String);
	}
	else if (const FTextProperty* TextProperty = CastField<FTextProperty>(Property))
	{
		return (DataType == LudeoDataType::Component);
	}
	else if (const FClassProperty* ClassProperty = CastField<FClassProperty>(Property))
	{
		return (DataType == LudeoDataType::String);
	}
	else if (const FStructProperty* StructProperty = CastField<FStructProperty>(Property))
	{
		return (DataType == LudeoDataType::Component);
	}
	else if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
	{
		return (DataType == LudeoDataType::UInt32);
	}
	else if (const FWeakObjectProperty* WeakObjectProperty = CastField<FWeakObjectProperty>(Property))
	{
		return (DataType == LudeoDataType::UInt32);
	}
	else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{
		return (DataType == LudeoDataType::Component);
	}
	else if (const FSetProperty* SetProperty = CastField<FSetProperty>(Property))
	{
		return (DataType == LudeoDataType::Component);
	}
	else if (const FMapProperty* MapProperty = CastField<FMapProperty>(Property))
	{
		return (DataType == LudeoDataType::Component);
	}
	
	return false;
}
