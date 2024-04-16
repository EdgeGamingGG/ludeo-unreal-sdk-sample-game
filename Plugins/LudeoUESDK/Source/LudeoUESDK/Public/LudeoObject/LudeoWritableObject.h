#pragma once

#include "CoreMinimal.h"

#include "LudeoObject/LudeoObject.h"
#include "LudeoPlayer/LudeoPlayer.h"

#include "LudeoRoom/LudeoRoomWriterTypes.h"

struct LUDEOUESDK_API FLudeoWritableObject : public FLudeoObject
{
	typedef TMap<const UObject*, FLudeoWritableObject> WritableObjectMapType;

public:
	FLudeoWritableObject(const UObject* Object, const FLudeoObjectHandle& InObjectHandle, const FLudeoRoomWriterHandle& InDataWriterHandle);

	~FLudeoWritableObject() = default;

	const UObject* GetObject() const;

	bool EnterObject() const;
	bool LeaveObject() const;

	bool EnterComponent(const FString& AttributeName) const;
	bool LeaveComponent() const;

	bool BindPlayer(const FLudeoPlayer& Player) const;
	bool UnbindPlayer() const;

	bool WriteData
	(
		const WritableObjectMapType& ObjectMap,
		const FLudeoPropertyFilter& PropertyFilter = FLudeoPropertyFilter()
	) const;

	bool WriteData(const TCHAR* AttributeName, const int8& Data) const;
	bool WriteData(const TCHAR* AttributeName, const int16& Data) const;
	bool WriteData(const TCHAR* AttributeName, const int32& Data) const;
	bool WriteData(const TCHAR* AttributeName, const int64& Data) const;

	bool WriteData(const TCHAR* AttributeName, const uint8& Data) const;
	bool WriteData(const TCHAR* AttributeName, const uint16& Data) const;
	bool WriteData(const TCHAR* AttributeName, const uint32& Data) const;
	bool WriteData(const TCHAR* AttributeName, const uint64& Data) const;

	bool WriteData(const TCHAR* AttributeName, const bool& Data) const;

	bool WriteData(const TCHAR* AttributeName, const float& Data) const;
	bool WriteData(const TCHAR* AttributeName, const double& Data) const;
	bool WriteData(const TCHAR* AttributeName, const FVector2D& Data) const;
	bool WriteData(const TCHAR* AttributeName, const FVector& Data) const;
	bool WriteData(const TCHAR* AttributeName, const FVector4& Data) const;
	bool WriteData(const TCHAR* AttributeName, const FRotator& Data) const;
	bool WriteData(const TCHAR* AttributeName, const FQuat& Data) const;
	bool WriteData(const TCHAR* AttributeName, const FTransform& Data) const;
	bool WriteData(const TCHAR* AttributeName, const TCHAR* Data) const;
	bool WriteData(const TCHAR* AttributeName, const FString& Data) const;
	bool WriteData(const TCHAR* AttributeName, const FName& Data) const;
	bool WriteData(const TCHAR* AttributeName, UClass* Data) const;

	bool WriteData(const TCHAR* AttributeName, const FLudeoObjectHandle& LudeoObjectHandle) const;
	bool WriteData(const TCHAR* AttributeName, const UObject* Data, const WritableObjectMapType& ObjectMap) const;

public:
	// Struct
	bool WriteData
	(
		const TCHAR* AttributeName,
		const UStruct* StructureType,
		const void* Structure,
		const WritableObjectMapType& ObjectMap,
		const FLudeoPropertyFilter& PropertyFilter = FLudeoPropertyFilter()
	) const;

	bool WriteData
	(
		const TCHAR* AttributeName,
		const void* PropertyContainer,
		const FProperty* Property,
		const WritableObjectMapType& ObjectMap
	) const;

private:
	bool InternalWriteData
	(
		const UStruct* StructureType,
		const void* StructureContainer,
		const WritableObjectMapType& ObjectMap,
		const FLudeoPropertyFilter& PropertyFilter
	) const;

private:
	FLudeoRoomWriterHandle RoomWriterHandle;
	TWeakObjectPtr<const UObject> WeakObject;
};
