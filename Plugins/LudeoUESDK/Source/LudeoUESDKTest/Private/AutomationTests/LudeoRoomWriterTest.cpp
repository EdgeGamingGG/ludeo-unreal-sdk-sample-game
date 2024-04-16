#include "LudeoTestUtility.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoRoomWriterSetSendSettingsTest,	"Ludeo.RoomWriter.SetSendSettings");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoRoomWriterCreateObjectTest,		"Ludeo.RoomWriter.CreateObject");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoRoomWriterDestroyObjectTest,		"Ludeo.RoomWriter.DestroyObject");

bool FLudeoRoomWriterSetSendSettingsTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetSendSettings,
			[](...)
			{
				return LudeoResult::Success;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoRoomWriter DataWriter(reinterpret_cast<LudeoHDataWriter>(0xBadBeefull));
		
		const FLudeoResult Result = DataWriter.SetSendSettings({});

		TestTrue(TEXT("Result"), Result.IsSuccessful());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoRoomWriterCreateObjectTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_CreateObject,
			[](LudeoHDataWriter, const LudeoDataWriterCreateObjectParams*, LudeoObjectId* OutObjectID)
			{
				if (OutObjectID != nullptr)
				{
					*OutObjectID = 0xDeadBeefu;
				}

				return LudeoResult::Success;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoRoomWriter DataWriter(reinterpret_cast<LudeoHDataWriter>(0xBadBeefull));
	
		UObject* Object = NewObject<USceneComponent>();

		FLudeoRoomWriterCreateObjectParameters CreateObjectParameters;
		CreateObjectParameters.Object = Object;

		const TOptionalWithLudeoResult<FLudeoWritableObject> Result = DataWriter.CreateObject(CreateObjectParameters);

		TestTrue(TEXT("Create Object Result"), Result.IsSuccessful());

		if(Result.IsSuccessful())
		{
			TestEqual
			(
				TEXT("Object Handle"),
				static_cast<LudeoObjectId>(static_cast<FLudeoObjectHandle>(Result.GetValue())),
				static_cast<LudeoObjectId>(0xDeadBeefu)
			);
		}

		Object->ConditionalBeginDestroy();
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoRoomWriterDestroyObjectTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_DestroyObject,
			[](...)
			{
				return LudeoResult::Success;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FLudeoRoomWriter RoomWriter(reinterpret_cast<LudeoHDataWriter>(0xBadBeefull));
		
		bool bHasReceivedResult = false;

		FLudeoRoomWriterDestroyObjectParameters DestroyObjectParameters;
		DestroyObjectParameters.ObjectHandle = static_cast<LudeoObjectId>(0xDeadBeefu);
		
		TestTrue(TEXT("Destroy Object Result"), RoomWriter.DestroyObject(DestroyObjectParameters).IsSuccessful());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

#endif //WITH_DEV_AUTOMATION_TESTS
