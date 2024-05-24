#pragma once

#include "CoreMinimal.h"

#include "LudeoUESDK/LudeoObject/LudeoObject.h"
#include "LudeoUESDK/LudeoPlayer/LudeoPlayer.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoomWriterTypes.h"

class APlayerState;

/* A data writer for writing states for the mapped UObject */
class LUDEOUESDK_API FLudeoWritableObject : public FLudeoObject
{
public:
	typedef TMap<const UObject*, FLudeoWritableObject> WritableObjectMapType;

public:
	FLudeoWritableObject(const UObject* Object, const FLudeoObjectHandle& InObjectHandle, const FLudeoRoomWriterHandle& InDataWriterHandle);
	~FLudeoWritableObject();

	/* Get the UObject mapped to this writable object from its internal weak pointer */
	const UObject* GetObject() const;

	/* Pushes the current object to the DataWriter context stack */
	bool EnterObject() const;

	/* Pop the object from the DataWriter context stack */
	bool LeaveObject() const;

	/* Pushes the struct associated with the AttributeName to the DataWriter context stack */
	bool EnterComponent(const char* AttributeName) const;

	/* Pop the struct from the DataWriter context stack */
	bool LeaveComponent() const;

	/* Bind the current object to a player */
	bool BindPlayer(const char* PlayerID) const;

	/* Unbind the object to a player */
	bool UnbindPlayer() const;

public:
	bool WriteData
	(
		const WritableObjectMapType& ObjectMap,
		const FLudeoObjectPropertyFilter& PropertyFilter = FLudeoObjectPropertyFilter()
	) const;

public:
	bool WriteData(const char* AttributeName, const int8& Data) const;
	bool WriteData(const char* AttributeName, const int16& Data) const;
	bool WriteData(const char* AttributeName, const int32& Data) const;
	bool WriteData(const char* AttributeName, const int64& Data) const;

	bool WriteData(const char* AttributeName, const uint8& Data) const;
	bool WriteData(const char* AttributeName, const uint16& Data) const;
	bool WriteData(const char* AttributeName, const uint32& Data) const;
	bool WriteData(const char* AttributeName, const uint64& Data) const;

	bool WriteData(const char* AttributeName, const bool& Data) const;

	bool WriteData(const char* AttributeName, const float& Data) const;
	bool WriteData(const char* AttributeName, const double& Data) const;
	bool WriteData(const char* AttributeName, const FVector2D& Data) const;
	bool WriteData(const char* AttributeName, const FVector& Data) const;
	bool WriteData(const char* AttributeName, const FVector4& Data) const;
	bool WriteData(const char* AttributeName, const FRotator& Data) const;
	bool WriteData(const char* AttributeName, const FQuat& Data) const;
	bool WriteData(const char* AttributeName, const FTransform& Data) const;
	bool WriteData(const char* AttributeName, const char* Data) const;
	bool WriteData(const char* AttributeName, const TCHAR* Data) const;
	bool WriteData(const char* AttributeName, const FString& Data) const;
	bool WriteData(const char* AttributeName, const FName& Data) const;
	bool WriteData(const char* AttributeName, const FText& Data) const;
	bool WriteData(const char* AttributeName, UClass* Data) const;

	bool WriteData(const char* AttributeName, const FLudeoObjectHandle& Data) const;
	bool WriteData(const char* AttributeName, const UObject* Data, const WritableObjectMapType& ObjectMap) const;
	bool WriteData(const char* AttributeName, const FWeakObjectPtr& Data, const WritableObjectMapType& ObjectMap) const;

	bool WriteData
	(
		const char* AttributeName,
		const void* PropertyContainer,
		const FProperty* Property,
		const WritableObjectMapType& ObjectMap
	) const;

	// Struct
	bool WriteData
	(
		const char* AttributeName,
		const UStruct* StructureType,
		const void* Structure,
		const WritableObjectMapType& ObjectMap,
		const FLudeoObjectPropertyFilter& PropertyFilter = FLudeoObjectPropertyFilter()
	) const;

private:
	bool InternalWriteData
	(
		const UStruct* StructureType,
		const void* StructureContainer,
		const WritableObjectMapType& ObjectMap,
		const FLudeoObjectPropertyFilter& PropertyFilter
	) const;

private:
	FLudeoRoomWriterHandle RoomWriterHandle;
	TWeakObjectPtr<const UObject> WeakObject;
};
