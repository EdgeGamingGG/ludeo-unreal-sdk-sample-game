#include "LudeoObject/LudeoObject.h"

FORCEINLINE uint32 GetTypeHash(const FLudeoObjectHandle& ObjectHandle)
{
	return GetTypeHash(static_cast<LudeoObjectId>(ObjectHandle));
}

FORCEINLINE uint32 GetTypeHash(const FLudeoObject& Object)
{
	return GetTypeHash(static_cast<FLudeoObjectHandle>(Object));
}
