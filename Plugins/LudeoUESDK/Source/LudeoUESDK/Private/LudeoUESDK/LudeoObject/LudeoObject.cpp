#include "LudeoUESDK/LudeoObject/LudeoObject.h"

FLudeoObject::FLudeoObject(const FLudeoObjectHandle InObjectHandle) :
	ObjectHandle(InObjectHandle)
{
	check(ObjectHandle.IsValid());
}


FLudeoObject::~FLudeoObject()
{

}

uint32 GetTypeHash(const FLudeoObjectHandle& ObjectHandle)
{
	return GetTypeHash(static_cast<LudeoObjectId>(ObjectHandle));
}

uint32 GetTypeHash(const FLudeoObject& Object)
{
	return GetTypeHash(static_cast<FLudeoObjectHandle>(Object));
}
