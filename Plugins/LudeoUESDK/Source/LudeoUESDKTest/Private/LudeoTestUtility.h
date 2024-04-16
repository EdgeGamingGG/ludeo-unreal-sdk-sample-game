#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS
#include <Ludeo/Session.h>
#include <Ludeo/Room.h>
#include <Ludeo/DataReader.h>
#include <Ludeo/DataWriter.h>

#include "LudeoAllTypes.h"

#undef ensure

#include "polyhook2/Detour/x64Detour.hpp"

#include "Windows/AllowWindowsPlatformTypes.h"
	#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"

class FLudeoAutomationTest : public FAutomationTestBase
{
public:
	FLudeoAutomationTest(const FString& InName, const bool bInComplexTask) :
		FAutomationTestBase(InName, bInComplexTask)
	{

	}
};

#define IMPLEMENT_LUDEO_AUTOMATION_TEST(ClassName, TestCategory) IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(ClassName, FLudeoAutomationTest, TestCategory, EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::StressFilter)

#define SETUP_DETOUR_FUNCTION(FunctionName, Lambda)	GET_LUDEO_DLL_FUNCTION_MEMORY_ADDRESS(FunctionName), +Lambda

#define GET_LUDEO_DLL_FUNCTION_MEMORY_ADDRESS(FunctionName) GetProcAddress(GetModuleHandleA(LUDEOSDK_DLL_NAME), #FunctionName)

#define TEST_LUDEO_RESULT(Description, Result, ExpectedResultCode)						\
{																						\
	if (!(Result == ExpectedResultCode))												\
	{																					\
		AddError																		\
		(																				\
			FString::Printf																\
			(																			\
				TEXT("[%s] Expected result to be %s, but actual result is %s"),			\
				Description,															\
				UTF8_TO_TCHAR(FLudeoResult(ExpectedResultCode).ToString().GetData()),	\
				UTF8_TO_TCHAR(Result.ToString().GetData())								\
			)																			\
		);																				\
	}																					\
}

template<typename OutputType, typename InputType>
OutputType ForceCast(InputType Input)
{
	const union
	{
		InputType  InputTypeOutput;
		OutputType OutputTypeOutput;

	} Union = { Input };

	return Union.OutputTypeOutput;
};

template<typename ConstructorFunctionType, typename DestructorFunctionType>
struct FScopedFuctionExecutionGuard
{
	FScopedFuctionExecutionGuard(ConstructorFunctionType ConstructorFucntion, DestructorFunctionType InDestructorFunction) :
		DestructorFunction(InDestructorFunction)
	{
		ConstructorFucntion();
	}

	~FScopedFuctionExecutionGuard()
	{
		DestructorFunction();
	}

private:
	DestructorFunctionType DestructorFunction;
};

class FScopedDetourFunctionGuard
{
public:
	template<typename ... ArgumentsType>
	FScopedDetourFunctionGuard(ArgumentsType&&... Arguments) :
		SharedTrampoline(0),
		bIsDetoured(false)
	{
		bIsDetoured = Detour(Forward<ArgumentsType>(Arguments)...);

		if (!bIsDetoured)
		{
			Finalize();
		}
	}

	FScopedDetourFunctionGuard(FScopedDetourFunctionGuard&& Other) :
		SharedTrampoline(Other.SharedTrampoline),
		bIsDetoured(Other.bIsDetoured)
	{
		DetourFunctionCollection = MoveTemp(Other.DetourFunctionCollection);
	}

	~FScopedDetourFunctionGuard()
	{
		Finalize();
	}

	bool IsDetoured() const
	{
		return bIsDetoured;
	}

private:
	void Finalize()
	{
		for (PLH::x64Detour& DetourFunction : DetourFunctionCollection)
		{
			if (DetourFunction.isHooked())
			{
				DetourFunction.unHook();
			}
		}

		DetourFunctionCollection.Empty();
	}

	template<
		typename OriginalFunctionType, typename ReplacementFunctionType,
		typename ... RemainingArgumentsType
	>
	bool Detour
	(
		OriginalFunctionType OriginalFunctionPointer,
		ReplacementFunctionType ReplacementFunction,
		RemainingArgumentsType&&... RemainingArguments
	)
	{
		DetourFunctionCollection.AddUninitialized();

		PLH::x64Detour* DetourFunction = new (&DetourFunctionCollection.Last()) PLH::x64Detour
		(
			reinterpret_cast<uint64>(OriginalFunctionPointer),
			reinterpret_cast<uint64>(ReplacementFunction),
			&SharedTrampoline
		);

		const bool bIsHooked = DetourFunction->hook() && Detour(Forward<RemainingArgumentsType>(RemainingArguments)...);
		
		return bIsHooked;
	}

	bool Detour() const
	{
		return true;
	}

private:
	TArray<PLH::x64Detour>  DetourFunctionCollection;
	uint64					SharedTrampoline;
	bool					bIsDetoured;
};

#endif
