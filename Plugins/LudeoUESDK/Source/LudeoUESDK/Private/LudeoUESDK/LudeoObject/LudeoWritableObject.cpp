#include "LudeoUESDK/LudeoObject/LudeoWritableObject.h"

#include "GameFramework/PlayerState.h"

#include <Ludeo/DataWriter.h>
#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoScopedGuard.h"

// Suppress unreal wrong deprecated warning
#undef UInt16Property
#undef UInt32Property
#undef UInt64Property

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
bool GenericWriteData(const FLudeoRoomWriterHandle& RoomWriterHandle, const TCHAR* AttributeName, const DataType& Data)
{
	check(AttributeName != nullptr);

	const FLudeoResult ConditionalSetCurrentResult = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (ConditionalSetCurrentResult.IsSuccessful())
	{
		return (ludeo_DataWriter_Set(TCHAR_TO_UTF8(AttributeName), Data) == LUDEO_TRUE);
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

bool FLudeoWritableObject::EnterComponent(const TCHAR* AttributeName) const
{
	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_EnterComponent(TCHAR_TO_UTF8(AttributeName)) == LUDEO_TRUE);
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

bool FLudeoWritableObject::BindPlayer(const TCHAR* PlayerID) const
{
	const FLudeoResult Result = ConditionalRoomWriterSetCurrent(RoomWriterHandle);

	if (Result.IsSuccessful())
	{
		return (ludeo_DataWriter_SetPlayerBinding(TCHAR_TO_UTF8(PlayerID)) == LUDEO_TRUE);
	}

	return false;
}

bool FLudeoWritableObject::UnbindPlayer() const
{
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
	const FLudeoPropertyFilter& PropertyFilter
) const
{
	if(const UObject* Data = WeakObject.Get())
	{
		return InternalWriteData(Data->GetClass(), Data, ObjectMap, PropertyFilter);
	}
	
	return false;
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const int8& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const int16& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const int32& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const int64& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const uint8& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const uint16& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const uint32& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const uint64& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const bool& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const float& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const double& Data) const
{
	return GenericWriteData(RoomWriterHandle, AttributeName, Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FVector2D& Data) const
{
	return WriteData(AttributeName, FLudeoVector2D::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FVector& Data) const
{
	return WriteData(AttributeName, FLudeoVector::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FVector4& Data) const
{
	return WriteData(AttributeName, FLudeoVector4::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FRotator& Data) const
{
	return WriteData(AttributeName, FLudeoRotator::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FQuat& Data) const
{
	return WriteData(AttributeName, FLudeoQuaterion::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FTransform& Data) const
{
	return WriteData(AttributeName, FLudeoTransform::StaticStruct()->GetSuperStruct(), &Data, {});
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const TCHAR* Data) const
{
	const FTCHARToUTF8 StringConverter(Data);

	return GenericWriteData(RoomWriterHandle, AttributeName, StringConverter.Get());
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FString& Data) const
{
	return WriteData(AttributeName, *Data);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FName& Data) const
{
	return WriteData(AttributeName, *Data.ToString());
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, UClass* Data) const
{
	const FString ClassName = (Data != nullptr ? Data->GetName() : TEXT(""));

	return WriteData(AttributeName, *ClassName);
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const FLudeoObjectHandle& LudeoObjectHandle) const
{
	return WriteData(AttributeName, static_cast<LudeoObjectId>(LudeoObjectHandle));
}

bool FLudeoWritableObject::WriteData(const TCHAR* AttributeName, const UObject* Data, const WritableObjectMapType& ObjectMap) const
{
	const FLudeoObjectHandle LudeoObjectHandle = [&]()
	{
		if (Data != nullptr)
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

bool FLudeoWritableObject::WriteData
(
	const TCHAR* AttributeName,
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
		const UObject* SubObject = ObjectProperty->GetObjectPropertyValue_InContainer(PropertyContainer);

		bIsDataWrittenSuccessfully = WriteData(AttributeName, SubObject, ObjectMap);
	}
	else if (const FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property))
	{	
		bIsDataWrittenSuccessfully = true;

		FScriptArrayHelper ScriptArrayHelper(ArrayProperty, ArrayProperty->ContainerPtrToValuePtr<void>(PropertyContainer));

		if(ScriptArrayHelper.Num() > 0)
		{
			const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject, false> EnterComponentGuard(*this, AttributeName);

			bIsDataWrittenSuccessfully = EnterComponentGuard.HasEnteredComponent();

			if(bIsDataWrittenSuccessfully)
			{
				bIsDataWrittenSuccessfully = WriteData(TEXT("ArraySize"), ScriptArrayHelper.Num());

				for (int32 i = 0; (bIsDataWrittenSuccessfully && i < ScriptArrayHelper.Num()); ++i)
				{
					bIsDataWrittenSuccessfully = WriteData
					(
						*FString::FromInt(i),
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
			const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject, false> EnterComponentGuard(*this, AttributeName);

			bIsDataWrittenSuccessfully = EnterComponentGuard.HasEnteredComponent();

			if (bIsDataWrittenSuccessfully)
			{
				bIsDataWrittenSuccessfully = WriteData(TEXT("SetSize"), ScriptSetHelper.Num());

				{
					int32 ElementNumber = 0;

					for (FScriptSetHelper::FIterator Itr(ScriptSetHelper);  (bIsDataWrittenSuccessfully && Itr); ++Itr)
					{
						bIsDataWrittenSuccessfully = WriteData
						(
							*FString::FromInt(ElementNumber),
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
			const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject, false> EnterComponentGuard(*this, AttributeName);

			bIsDataWrittenSuccessfully = EnterComponentGuard.HasEnteredComponent();

			if (bIsDataWrittenSuccessfully)
			{
				bIsDataWrittenSuccessfully = WriteData(TEXT("MapSize"), ScriptMapHelper.Num());

				{
					int32 PairNumber = 0;

					for (FScriptMapHelper::FIterator Itr(ScriptMapHelper); (bIsDataWrittenSuccessfully && Itr); ++Itr)
					{
						bIsDataWrittenSuccessfully = WriteData
						(
							*FString::Printf(TEXT("%Key_d"), PairNumber),
							ScriptMapHelper.GetKeyPtr(*Itr),
							ScriptMapHelper.GetKeyProperty(),
							ObjectMap
						);

						bIsDataWrittenSuccessfully = bIsDataWrittenSuccessfully && WriteData
						(
							*FString::Printf(TEXT("Value_d"), PairNumber),
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

bool FLudeoWritableObject::WriteData
(
	const TCHAR* AttributeName,
	const UStruct* StructureType,
	const void* Structure,
	const WritableObjectMapType& ObjectMap,
	const FLudeoPropertyFilter& PropertyFilter
) const
{
	const FScopedLudeoDataReadWriteEnterComponentGuard<FLudeoWritableObject, false> EnterComponentGuard(*this, AttributeName);

	if(EnterComponentGuard.HasEnteredComponent())
	{
		return InternalWriteData(StructureType, Structure, ObjectMap, PropertyFilter);
	}

	return false;
}

bool FLudeoWritableObject::InternalWriteData
(
	const UStruct* StructureType,
	const void* StructureContainer,
	const WritableObjectMapType& ObjectMap,
	const FLudeoPropertyFilter& PropertyFilter
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
			bIsAllDataWrittenSuccessfully = WriteData(*Property->GetName(), StructureContainer, Property, ObjectMap);
		}
	}

	return bIsAllDataWrittenSuccessfully;
}
