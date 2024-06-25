#pragma once

#include "CoreMinimal.h"

#include "LudeoUESDK/Ludeo/LudeoTypes.h"
#include "LudeoUESDK/LudeoObject/LudeoObject.h"

class LUDEOUESDK_API FLudeoReadableObject : public FLudeoObject
{
public:
	typedef TMap<FLudeoReadableObject, UObject*> ReadableObjectMapType;
	typedef TFunction<bool(const uint32 NumberOfAttribute, const char* AttributeName, const LudeoDataType DataType)> IteratePredicateType;

public:
	FLudeoReadableObject(const FLudeoObjectHandle InObjectHandle, const FLudeoHandle& InLudeoHandle);
	~FLudeoReadableObject();

	/* Pushes the current object to the DataReader context stack */
	bool EnterObject() const;

	/* Pop the object from the DataReader context stack */
	bool LeaveObject() const;

	/* Pushes the struct associated with the AttributeName to the DataReader context stack */
	bool EnterComponent(const char* AttributeName) const;

	/* Pop the struct from the DataReader context stack */
	bool LeaveComponent() const;

	// Check if an attribute exists
	bool ExistAttribute(const char* AttributeName) const;

	// Get data type of an attribute
	TOptionalWithLudeoResult<LudeoDataType> GetAttributeDataType(const char* AttributeName) const;

	// Iterate attributes in the current context
	TOptionalWithLudeoResult<uint32> Iterate(const IteratePredicateType& Predicate) const;

	bool ReadData
	(
		const UObject* Data,
		const ReadableObjectMapType& ObjectMap,
		const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
	) const;

	bool ReadData(const char* AttributeName, int8& Data) const;
	bool ReadData(const char* AttributeName, int16& Data) const;
	bool ReadData(const char* AttributeName, int32& Data) const;
	bool ReadData(const char* AttributeName, int64& Data) const;

	bool ReadData(const char* AttributeName, uint8& Data) const;
	bool ReadData(const char* AttributeName, uint16& Data) const;
	bool ReadData(const char* AttributeName, uint32& Data) const;
	bool ReadData(const char* AttributeName, uint64& Data) const;

	bool ReadData(const char* AttributeName, bool& Data) const;

	bool ReadData(const char* AttributeName, float& Data) const;
	bool ReadData(const char* AttributeName, double& Data) const;
	bool ReadData(const char* AttributeName, FVector2D& Data) const;
	bool ReadData(const char* AttributeName, FVector& Data) const;
	bool ReadData(const char* AttributeName, FVector4& Data) const;
	bool ReadData(const char* AttributeName, FRotator& Data) const;
	bool ReadData(const char* AttributeName, FQuat& Data) const;
	bool ReadData(const char* AttributeName, FTransform& Data) const;
	bool ReadData(const char* AttributeName, FString& Data) const;
	bool ReadData(const char* AttributeName, FName& Data) const;
	bool ReadData(const char* AttributeName, FText& Data) const;
	bool ReadData(const char* AttributeName, UClass*& Data, FString* ClassPathName = nullptr) const;
	bool ReadData(const char* AttributeName, FLudeoObjectHandle& Data) const;
	bool ReadData(const char* AttributeName, UObject*& Data, const ReadableObjectMapType& ObjectMap) const;
	bool ReadData(const char* AttributeName, FWeakObjectPtr& Data, const ReadableObjectMapType& ObjectMap) const;
	
	// Struct
	bool ReadData
	(
		const char* AttributeName,
		const UStruct* StructureType,
		void* Structure,
		const ReadableObjectMapType& ObjectMap,
		const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
	) const;

	bool ReadData
	(
		const char* AttributeName,
		const void* PropertyContainer,
		const FProperty* Property,
		const ReadableObjectMapType& ObjectMap,
		const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
	) const;

private:
	bool InternalReadData
	(
		const UStruct* StructureType,
		void* StructureContainer,
		const ReadableObjectMapType& ObjectMap,
		const TOptional<FLudeoObjectPropertyFilter>& PropertyFilter
	) const;

	bool IsPropertyMatchingDataType(const FProperty* Property, const LudeoDataType DataType) const;

private:
	FLudeoHandle LudeoHandle;
};
