#include "LudeoTestUtility.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoWritableObjectEnterObjectTest,		"Ludeo.WritableObject.EnterObject");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoWritableObjectLeaveObjectTest,		"Ludeo.WritableObject.LeaveObject");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoWritableObjectEnterComponentTest,		"Ludeo.WritableObject.EnterComponent");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoWritableObjectLeaveComponentTest,		"Ludeo.WritableObject.LeaveComponent");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoWritableObjectBindPlayerTest,			"Ludeo.WritableObject.BindPlayer");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoWritableObjectUnbindPlayerTest,		"Ludeo.WritableObject.UnbindPlayer");
IMPLEMENT_LUDEO_AUTOMATION_TEST(FLudeoWritableObjectWriteDataTest,			"Ludeo.WritableObject.WriteData");

bool FLudeoWritableObjectEnterObjectTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_EnterObject,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoWritableObject WritableObject(nullptr, LUDEO_INVALID_OBJECTID, nullptr);

		TestTrue(TEXT("Enter object"), WritableObject.EnterObject());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoWritableObjectLeaveObjectTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_LeaveObject,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoWritableObject WritableObject(nullptr, LUDEO_INVALID_OBJECTID, nullptr);

		TestTrue(TEXT("Leave object"), WritableObject.LeaveObject());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoWritableObjectEnterComponentTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_EnterComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoWritableObject WritableObject(nullptr, LUDEO_INVALID_OBJECTID, nullptr);

		TestTrue(TEXT("Leave object"), WritableObject.EnterComponent({}));
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoWritableObjectLeaveComponentTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_LeaveComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoWritableObject WritableObject(nullptr, LUDEO_INVALID_OBJECTID, nullptr);

		TestTrue(TEXT("Leave object"), WritableObject.LeaveComponent());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}


bool FLudeoWritableObjectBindPlayerTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetPlayerBinding,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoWritableObject WritableObject(nullptr, LUDEO_INVALID_OBJECTID, nullptr);
		
		TestTrue(TEXT("Bind Player"), WritableObject.BindPlayer(FLudeoPlayer(nullptr)));
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoWritableObjectUnbindPlayerTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetPlayerBinding,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		const FLudeoWritableObject WritableObject(nullptr, LUDEO_INVALID_OBJECTID, nullptr);

		TestTrue(TEXT("Unbind Player"), WritableObject.UnbindPlayer());
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}

bool FLudeoWritableObjectWriteDataTest::RunTest(const FString& Parameters)
{
	const FScopedDetourFunctionGuard DetourFunction
	(
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetCurrent,
			[](...)
			{
				return LudeoResult::Success;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_EnterComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_LeaveComponent,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetInt8,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetInt16,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetInt32,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetInt64,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetUInt8,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetUInt16,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetUInt32,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetUInt64,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetBool,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetFloat,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetDouble,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetVec3Float,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetVec4Float,
			[](...)
			{
				return LUDEO_TRUE;
			}
		),
		SETUP_DETOUR_FUNCTION
		(
			ludeo_DataWriter_SetString,
			[](...)
			{
				return LUDEO_TRUE;
			}
		)
	);

	if (DetourFunction.IsDetoured())
	{
		float Data[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		FVector2D Vector2D;
		FVector Vector;
		FVector4 Vector4;
		FRotator Rotator;
		FQuat Quaternion;
		FTransform Transform;
		FString String;
		FName Name;
		UClass* ObjectClass = AActor::StaticClass();

		const FLudeoWritableObject WritableObject(nullptr, LUDEO_INVALID_OBJECTID, nullptr);

		TestTrue(TEXT("Set Int8 Data"),					WritableObject.WriteData(TEXT("Bad Beef"), static_cast<int8>(Data[0])));
		TestTrue(TEXT("Set Int16 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<int16>(Data[0])));
		TestTrue(TEXT("Set int32 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<int32>(Data[0])));
		TestTrue(TEXT("Set Int64 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<int64>(Data[0])));
		TestTrue(TEXT("Set UInt8 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<uint8>(Data[0])));
		TestTrue(TEXT("Set UInt16 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<uint16>(Data[0])));
		TestTrue(TEXT("Set Uint32 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<uint32>(Data[0])));
		TestTrue(TEXT("Set UInt64 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<uint64>(Data[0])));
		TestTrue(TEXT("Set bool Data"),					WritableObject.WriteData(TEXT("Bad Beef"), static_cast<bool>(Data[0])));
		TestTrue(TEXT("Set Float Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<float>(Data[0])));
		TestTrue(TEXT("Set Double Data"),				WritableObject.WriteData(TEXT("Bad Beef"), static_cast<double>(Data[0])));
		TestTrue(TEXT("Set Vector2D Data"),				WritableObject.WriteData(TEXT("Bad Beef"), Vector2D));
		TestTrue(TEXT("Set Vector Data"),				WritableObject.WriteData(TEXT("Bad Beef"), Vector));
		TestTrue(TEXT("Set Vector4 Data"),				WritableObject.WriteData(TEXT("Bad Beef"), Vector4));
		TestTrue(TEXT("Set Rotator Data"),				WritableObject.WriteData(TEXT("Bad Beef"), Rotator));
		TestTrue(TEXT("Set Quaternion Data"),			WritableObject.WriteData(TEXT("Bad Beef"), Quaternion));
		TestTrue(TEXT("Set Transform Data"),			WritableObject.WriteData(TEXT("Bad Beef"), Transform));
		TestTrue(TEXT("Set String Data"),				WritableObject.WriteData(TEXT("Bad Beef"), TEXT("Dead Beef")));
		TestTrue(TEXT("Set String Data"),				WritableObject.WriteData(TEXT("Bad Beef"), *String));
		TestTrue(TEXT("Set String Data"),				WritableObject.WriteData(TEXT("Bad Beef"), String));
		TestTrue(TEXT("Set Name Data"),					WritableObject.WriteData(TEXT("Bad Beef"), Name));
		TestTrue(TEXT("Set Class Data"),				WritableObject.WriteData(TEXT("Bad Beef"), ObjectClass));
		TestTrue(TEXT("Set Ludeo Object Handle Data"),	WritableObject.WriteData(TEXT("Bad Beef"), reinterpret_cast<FLudeoObjectHandle&>(Data[0])));
	}

	return (DetourFunction.IsDetoured() && !HasAnyErrors());
}
#endif //WITH_DEV_AUTOMATION_TESTS
