#include "LudeoTestUtility.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoReadableObjectEnterObjectTest,		"Ludeo.ReadableObject.EnterObject");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoReadableObjectLeaveObjectTest,		"Ludeo.ReadableObject.LeaveObject");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoReadableObjectEnterComponentTest,		"Ludeo.ReadableObject.EnterComponent");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoReadableObjectLeaveComponentTest,		"Ludeo.ReadableObject.LeaveComponent");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoReadableObjectReadDataTest,			"Ludeo.ReadableObject.ReadData");

bool FLudeoReadableObjectEnterObjectTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_EnterObject,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoReadableObject ReadableObject(0xBadBeef, reinterpret_cast<LudeoHDataReader>(0xBadBeef));

		TestTrue(TEXT("Enter object"), ReadableObject.EnterObject());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoReadableObjectLeaveObjectTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_LeaveObject,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoReadableObject ReadableObject(0xBadBeef, reinterpret_cast<LudeoHDataReader>(0xBadBeef));

		TestTrue(TEXT("Leave object"), ReadableObject.LeaveObject());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoReadableObjectEnterComponentTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_EnterComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoReadableObject ReadableObject(0xBadBeef, reinterpret_cast<LudeoHDataReader>(0xBadBeef));

		TestTrue(TEXT("Leave object"), ReadableObject.EnterComponent({}));
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoReadableObjectLeaveComponentTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_LeaveComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoReadableObject ReadableObject(0xBadBeef, reinterpret_cast<LudeoHDataReader>(0xBadBeef));

		TestTrue(TEXT("Leave object"), ReadableObject.LeaveComponent());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoReadableObjectReadDataTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_EnterComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_LeaveComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetInt8,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetInt16,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetInt32,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetInt64,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetUInt8,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetUInt16,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetUInt32,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetUInt64,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetBool,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetFloat,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetDouble,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetVec3Float,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetVec4Float,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetString,
			[](const char*, char* OutValue)
			{
				const FTCHARToUTF8 Converter(*AActor::StaticClass()->GetName());

				FMemory::Memcpy(OutValue, Converter.Get(), Converter.Length());

				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataReader_GetSize,
			[](const char*, uint32_t* OutSize)
			{
				const FTCHARToUTF8 Converter(*AActor::StaticClass()->GetName());
				*OutSize = static_cast<uint32_t>(Converter.Length());

				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		FVector Vector2D;
		FVector Vector;
		FVector4 Vector4;
		FRotator Rotator;
		FQuat Quaternion;
		FTransform Transform;
		FString String;
		FName Name;
		UClass* ObjectClass = nullptr;

		char DataBuffer[64];

		const FLudeoReadableObject ReadableObject(0xBadBeef, reinterpret_cast<LudeoHDataReader>(0xBadBeef));

		TestTrue(TEXT("Get Int8 Data"),					ReadableObject.ReadData("Bad Beef", reinterpret_cast<int8&>(DataBuffer[0])));
		TestTrue(TEXT("Get Int16 Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<int16&>(DataBuffer[0])));
		TestTrue(TEXT("Get int32 Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<int32&>(DataBuffer[0])));
		TestTrue(TEXT("Get Int64 Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<int64&>(DataBuffer[0])));
		TestTrue(TEXT("Get UInt8 Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<uint8&>(DataBuffer[0])));
		TestTrue(TEXT("Get UInt16 Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<uint16&>(DataBuffer[0])));
		TestTrue(TEXT("Get Uint32 Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<uint32&>(DataBuffer[0])));
		TestTrue(TEXT("Get UInt64 Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<uint64&>(DataBuffer[0])));
		TestTrue(TEXT("Get bool Data"),					ReadableObject.ReadData("Bad Beef", reinterpret_cast<bool&>(DataBuffer[0])));
		TestTrue(TEXT("Get Float Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<float&>(DataBuffer[0])));
		TestTrue(TEXT("Get Double Data"),				ReadableObject.ReadData("Bad Beef", reinterpret_cast<double&>(DataBuffer[0])));
		TestTrue(TEXT("Get Vector2D Data"),				ReadableObject.ReadData("Bad Beef", Vector2D));
		TestTrue(TEXT("Get Vector Data"),				ReadableObject.ReadData("Bad Beef", Vector));
		TestTrue(TEXT("Get Vector4 Data"),				ReadableObject.ReadData("Bad Beef", Vector4));
		TestTrue(TEXT("Get Rotator Data"),				ReadableObject.ReadData("Bad Beef", Rotator));
		TestTrue(TEXT("Get Quaternion Data"),			ReadableObject.ReadData("Bad Beef", Quaternion));
		TestTrue(TEXT("Get Transform Data"),			ReadableObject.ReadData("Bad Beef", Transform));
		TestTrue(TEXT("Get String Data"),				ReadableObject.ReadData("Bad Beef", String));
		TestTrue(TEXT("Get Name Data"),					ReadableObject.ReadData("Bad Beef", Name));
		TestTrue(TEXT("Get Class Data"),				ReadableObject.ReadData("Bad Beef", ObjectClass));
		TestTrue(TEXT("Get Ludeo Object Handle Data"),	ReadableObject.ReadData("Bad Beef", reinterpret_cast<FLudeoObjectHandle&>(DataBuffer[0])));
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}
#endif //WITH_DEV_AUTOMATION_TESTS
