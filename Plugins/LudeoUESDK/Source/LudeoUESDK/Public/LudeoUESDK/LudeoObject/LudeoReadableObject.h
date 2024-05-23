#pragma once

#include "CoreMinimal.h"

#include "LudeoUESDK/Ludeo/LudeoTypes.h"
#include "LudeoUESDK/LudeoObject/LudeoObject.h"

class LUDEOUESDK_API FLudeoReadableObject : public FLudeoObject
{
public:
	typedef TMap<FLudeoReadableObject, UObject*> ReadableObjectMapType;

public:
	FLudeoReadableObject(const FLudeoObjectHandle InObjectHandle, const FLudeoHandle& InLudeoHandle);
	~FLudeoReadableObject();

	/* Pushes the current object to the DataReader context stack */
	bool EnterObject() const;

	/* Pop the object from the DataReader context stack */
	bool LeaveObject() const;

	/* Pushes the struct associated with the AttributeName to the DataReader context stack */
	bool EnterComponent(const TCHAR* AttributeName) const;

	/* Pop the struct from the DataReader context stack */
	bool LeaveComponent() const;

	// Check if an attribute exists
	bool ExistAttribute(const TCHAR* AttributeName) const;

	bool ReadData
	(
		const UObject* Data,
		const ReadableObjectMapType& ObjectMap,
		const FLudeoObjectPropertyFilter& PropertyFilter = FLudeoObjectPropertyFilter()
	) const;

	bool ReadData(const TCHAR* AttributeName, int8& Data) const;
	bool ReadData(const TCHAR* AttributeName, int16& Data) const;
	bool ReadData(const TCHAR* AttributeName, int32& Data) const;
	bool ReadData(const TCHAR* AttributeName, int64& Data) const;

	bool ReadData(const TCHAR* AttributeName, uint8& Data) const;
	bool ReadData(const TCHAR* AttributeName, uint16& Data) const;
	bool ReadData(const TCHAR* AttributeName, uint32& Data) const;
	bool ReadData(const TCHAR* AttributeName, uint64& Data) const;

	bool ReadData(const TCHAR* AttributeName, bool& Data) const;

	bool ReadData(const TCHAR* AttributeName, float& Data) const;
	bool ReadData(const TCHAR* AttributeName, double& Data) const;
	bool ReadData(const TCHAR* AttributeName, FVector2D& Data) const;
	bool ReadData(const TCHAR* AttributeName, FVector& Data) const;
	bool ReadData(const TCHAR* AttributeName, FVector4& Data) const;
	bool ReadData(const TCHAR* AttributeName, FRotator& Data) const;
	bool ReadData(const TCHAR* AttributeName, FQuat& Data) const;
	bool ReadData(const TCHAR* AttributeName, FTransform& Data) const;
	bool ReadData(const TCHAR* AttributeName, FString& Data) const;
	bool ReadData(const TCHAR* AttributeName, FName& Data) const;
	bool ReadData(const TCHAR* AttributeName, FText& Data) const;
	bool ReadData(const TCHAR* AttributeName, UClass*& Data, FString* ClassPathName = nullptr) const;
	bool ReadData(const TCHAR* AttributeName, FLudeoObjectHandle& Data) const;
	bool ReadData(const TCHAR* AttributeName, UObject*& Data, const ReadableObjectMapType& ObjectMap) const;
	bool ReadData(const TCHAR* AttributeName, FWeakObjectPtr& Data, const ReadableObjectMapType& ObjectMap) const;
	
	// Struct
	bool ReadData
	(
		const TCHAR* AttributeName,
		const UStruct* StructureType,
		void* Structure,
		const ReadableObjectMapType& ObjectMap,
		const FLudeoObjectPropertyFilter& PropertyFilter = FLudeoObjectPropertyFilter()
	) const;

	bool ReadData
	(
		const TCHAR* AttributeName,
		const void* PropertyContainer,
		const FProperty* Property,
		const ReadableObjectMapType& ObjectMap
	) const;

private:
	bool InternalReadData
	(
		const UStruct* StructureType,
		void* StructureContainer,
		const ReadableObjectMapType& ObjectMap,
		const FLudeoObjectPropertyFilter& PropertyFilter
	) const;

private:
	FLudeoHandle LudeoHandle;
};
