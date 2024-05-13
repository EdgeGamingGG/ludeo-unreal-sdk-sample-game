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
	bool EnterComponent(const TCHAR* AttributeName) const;

	/* Pop the struct from the DataWriter context stack */
	bool LeaveComponent() const;

	/* Bind the current object to a player */
	bool BindPlayer(const TCHAR* PlayerID) const;

	/* Unbind the object to a player */
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
