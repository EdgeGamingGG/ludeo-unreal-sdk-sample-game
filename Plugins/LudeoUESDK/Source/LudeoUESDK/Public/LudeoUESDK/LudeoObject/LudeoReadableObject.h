#pragma once

#include "CoreMinimal.h"

#include "LudeoUESDK/Ludeo/LudeoTypes.h"
#include "LudeoUESDK/LudeoObject/LudeoObject.h"

class LUDEOUESDK_API FLudeoReadableObject : public FLudeoObject
{
public:
	typedef TMap<FLudeoReadableObject, const UObject*> ReadableObjectMapType;

public:
	FLudeoReadableObject(const FLudeoObjectHandle InObjectHandle, const FLudeoHandle& InLudeoHandle) :
		FLudeoObject(InObjectHandle),
		LudeoHandle(InLudeoHandle)
	{

	}

	bool EnterObject() const;
	bool LeaveObject() const;

	bool EnterComponent(const TCHAR* AttributeName) const;
	bool LeaveComponent() const;

	bool ExistAttribute(const TCHAR* AttributeName) const;

	bool ReadData
	(
		const UObject* Data,
		const ReadableObjectMapType& ObjectMap,
		const FLudeoPropertyFilter& PropertyFilter = FLudeoPropertyFilter()
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
	bool ReadData(const TCHAR* AttributeName, UClass*& Data) const;

	bool ReadData(const TCHAR* AttributeName, FLudeoObjectHandle& Data) const;

	// Struct
	bool ReadData
	(
		const TCHAR* AttributeName,
		const UStruct* StructureType,
		void* Structure,
		const ReadableObjectMapType& ObjectMap,
		const FLudeoPropertyFilter& PropertyFilter = FLudeoPropertyFilter()
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
		const FLudeoPropertyFilter& PropertyFilter
	) const;

private:
	FLudeoHandle LudeoHandle;
};
