#pragma once

#include "CoreMinimal.h"

#include <Ludeo/Utils.h>

DECLARE_LOG_CATEGORY_EXTERN(LogLudeo, Log, All);

struct LUDEOUESDK_API FLudeoLogCategory
{
	FORCEINLINE FLudeoLogCategory(const LudeoLogCategory InLogCategory = LudeoLogCategory::All) :
		LogCategory(InLogCategory)
	{
	}

	FORCEINLINE operator LudeoLogCategory() const
	{
		return LogCategory;
	}

	FORCEINLINE TStringView<ANSICHAR> ToString() const
	{
		return ludeo_LogCategoryToString(LogCategory);
	}


private:
	LudeoLogCategory LogCategory;
};

struct FLudeoLogMessage
{
	ELogVerbosity::Type LogLevel;

	FLudeoLogCategory LogCategory;

	TStringView<ANSICHAR> Timestamp;

	TStringView<ANSICHAR> Message;
};

typedef void (*LogCallbackType)(const FLudeoLogMessage&);
