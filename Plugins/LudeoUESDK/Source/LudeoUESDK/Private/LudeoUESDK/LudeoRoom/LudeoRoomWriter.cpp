#include "LudeoUESDK/LudeoRoom/LudeoRoomWriter.h"

#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "EngineUtils.h"

#include <Ludeo/DataWriter.h>
#include <Ludeo/Utils.h>

#include "LudeoUESDK/LudeoObject/LudeoObject.h"
#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"

FLudeoRoomWriter::FLudeoRoomWriter(const FLudeoRoomWriterHandle& InRoomWriterHandle) :
	RoomWriterHandle(InRoomWriterHandle)
{
	check(RoomWriterHandle != nullptr);
}

FLudeoRoomWriter::~FLudeoRoomWriter()
{

}

FLudeoResult FLudeoRoomWriter::SetSendSettings(const FLudeoRoomWriterSetSendSettingsParameters& SetSendSettingsParameters) const
{
	check(SetSendSettingsParameters.SendInteralMS <= TNumericLimits<uint32_t>::Max());

	LudeoDataWriterSetSendSettingsParams InternalSetSendSettingsParams = Ludeo::create<LudeoDataWriterSetSendSettingsParams>();
	InternalSetSendSettingsParams.sendIntervalMs = static_cast<uint32_t>(SetSendSettingsParameters.SendInteralMS);

	return ludeo_DataWriter_SetSendSettings(RoomWriterHandle, &InternalSetSendSettingsParams);
}

TOptionalWithLudeoResult<FLudeoWritableObject> FLudeoRoomWriter::CreateObject
(
	const FLudeoRoomWriterCreateObjectParameters& CreateObjectParameters
) const
{
	check(CreateObjectParameters.Object != nullptr);

	TOptional<FTCHARToUTF8> ObjectTypeConverter;

	if (!CreateObjectParameters.ObjectType.IsEmpty())
	{
		const FString& ObjectType = CreateObjectParameters.ObjectType;
		ObjectTypeConverter.Emplace(ObjectType.GetCharArray().GetData(), ObjectType.GetCharArray().Num());
	}
	else
	{
		#if WITH_EDITOR
			const FString ObjectType = UWorld::RemovePIEPrefix(CreateObjectParameters.Object->GetClass()->GetPathName());
		#else
			const FString ObjectType = CreateObjectParameters.Object->GetClass()->GetPathName();
		#endif

		ObjectTypeConverter.Emplace(ObjectType.GetCharArray().GetData(), ObjectType.GetCharArray().Num());
	}

	LudeoDataWriterCreateObjectParams InternalCreateObjectParams = Ludeo::create<LudeoDataWriterCreateObjectParams>();
	InternalCreateObjectParams.objectType = ObjectTypeConverter.GetValue().Get();

	LudeoObjectId ObjectID = CreateObjectParameters.ObjectHandle;
	
	const FLudeoResult Result = ludeo_DataWriter_CreateObject
	(
		RoomWriterHandle,
		&InternalCreateObjectParams,
		&ObjectID
	);

	return TOptionalWithLudeoResult<FLudeoWritableObject>(Result, CreateObjectParameters.Object, ObjectID, RoomWriterHandle);
}

FLudeoResult FLudeoRoomWriter::DestroyObject(const FLudeoRoomWriterDestroyObjectParameters& DestroyObjectParameters) const
{
	LudeoDataWriterDestroyObjectParams InternalDestroyObjectParams = Ludeo::create<LudeoDataWriterDestroyObjectParams>();
	InternalDestroyObjectParams.objectId = DestroyObjectParameters.ObjectHandle;

	return ludeo_DataWriter_DestroyObject
	(
		RoomWriterHandle,
		&InternalDestroyObjectParams
	);
}

FLudeoResult FLudeoRoomWriter::SendAction(const FLudeoRoomWriterSendActionParameters& SendActionParameters) const
{
	const FTCHARToUTF8 PlayerIDStringConverter
	(
		SendActionParameters.PlayerID.GetCharArray().GetData(),
		SendActionParameters.PlayerID.GetCharArray().Num()
	);

	const FTCHARToUTF8 PlayerActionStringConverter
	(
		SendActionParameters.ActionName.GetCharArray().GetData(),
		SendActionParameters.ActionName.GetCharArray().Num()
	);

	LudeoDataWriterSendActionParams InternalSendActionParams = Ludeo::create<LudeoDataWriterSendActionParams>();
	InternalSendActionParams.playerId = PlayerIDStringConverter.Get();
	InternalSendActionParams.action = PlayerActionStringConverter.Get();

	return ludeo_DataWriter_SendAction(RoomWriterHandle, &InternalSendActionParams);
}
