#include "LudeoUESDK/LudeoObject/LudeoObject.h"

uint32 GetTypeHash(const FLudeoObjectHandle& ObjectHandle)
{
	return GetTypeHash(static_cast<LudeoObjectId>(ObjectHandle));
}

uint32 GetTypeHash(const FLudeoObject& Object)
{
	return GetTypeHash(static_cast<FLudeoObjectHandle>(Object));
}
