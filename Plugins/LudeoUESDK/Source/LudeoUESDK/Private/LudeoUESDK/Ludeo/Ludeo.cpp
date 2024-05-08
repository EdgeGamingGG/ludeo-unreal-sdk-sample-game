#include "LudeoUESDK/Ludeo/Ludeo.h"

#include <Ludeo/DataReader.h>
#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"

FLudeo::FLudeo(const FLudeoHandle& InLudeoHandle) :
	LudeoHandle(InLudeoHandle)
{
	const LudeoDataReaderGetInfoParams GetInfoParams = Ludeo::create<LudeoDataReaderGetInfoParams>();

	LudeoDataReaderInfo* pDataReaderInformation = nullptr;

	const FLudeoResult Result = ludeo_DataReader_GetInfo(LudeoHandle, &GetInfoParams, &pDataReaderInformation);
	check(Result.IsSuccessful());

	if(Result.IsSuccessful())
	{
		LudeoID = UTF8_TO_TCHAR(pDataReaderInformation->ludeoId);
		CreatorPlayerID = UTF8_TO_TCHAR(pDataReaderInformation->playerId);

		ludeo_DataReaderInfo_Release(pDataReaderInformation);
	}

	LudeoObjectInformationCollection = FetchLudeoObjectInformationCollection();
}

FLudeo::~FLudeo()
{
	ludeo_DataReader_Release(LudeoHandle);
}

const FLudeo* FLudeo::GetLudeoByLudeoHandle(const FLudeoHandle& LudeoHandle)
{
	if (FLudeoSession* Session = FLudeoSessionManager::StaticGetSessionByLudeoHandle(LudeoHandle))
	{
		return Session->GetLudeoByLudeoHandle(LudeoHandle);
	}

	return nullptr;
}

TArray<FLudeoObjectInformation> FLudeo::FetchLudeoObjectInformationCollection() const
{
	TArray<FLudeoObjectInformation> ObjectInformationCollection;

	LudeoObjectsInfo* InternalObjectsInformation = nullptr;

	const LudeoDataReaderGetObjectsInfoParams GetObjectsInformationParamters = Ludeo::create<LudeoDataReaderGetObjectsInfoParams>();

	const FLudeoResult Result = ludeo_DataReader_GetObjectsInfo(LudeoHandle, &GetObjectsInformationParamters, &InternalObjectsInformation);
	check(Result.IsSuccessful());

	if (Result.IsSuccessful())
	{
		check(InternalObjectsInformation != nullptr);

		ObjectInformationCollection.Reserve(InternalObjectsInformation->objectsCount);

		for (uint32 i = 0; i < static_cast<uint32>(InternalObjectsInformation->objectsCount); ++i)
		{
			const LudeoObjectInfo& InternalObjectInformation = InternalObjectsInformation->objects[i];

			const int32 Index = ObjectInformationCollection.AddUninitialized();
			new (&ObjectInformationCollection[Index].ReadableObject) FLudeoReadableObject(InternalObjectInformation.objectId, LudeoHandle);
			new (&ObjectInformationCollection[Index].ObjectType) FString(InternalObjectInformation.objectType);
		}

		ludeo_ObjectsInfo_Release(InternalObjectsInformation);
	}

	return MoveTemp(ObjectInformationCollection);
}
