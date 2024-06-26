#pragma once

#include "LudeoObjectTypes.h"

struct LUDEOUESDK_API FLudeoObject
{
public:
	FLudeoObject(const FLudeoObjectHandle InObjectHandle);
	~FLudeoObject();

	FORCEINLINE operator const FLudeoObjectHandle&() const
	{
		return ObjectHandle;
	}

	static FLudeoObject GetObjectByObjectHandle(const FLudeoObjectHandle& ObjectHandle)
	{
		return FLudeoObject(ObjectHandle);
	}

	FORCEINLINE bool operator==(const FLudeoObjectHandle& OtherObjectHandle) const
	{
		return (ObjectHandle == OtherObjectHandle);
	}

	FORCEINLINE bool IsValid() const
	{
		return ObjectHandle.IsValid();
	}

private:
	FLudeoObjectHandle ObjectHandle;
};

LUDEOUESDK_API uint32 GetTypeHash(const FLudeoObjectHandle& ObjectHandle);
LUDEOUESDK_API uint32 GetTypeHash(const FLudeoObject& Object);
