#pragma once

#include "LudeoLogTypes.h"

#include <Ludeo/Init.h>

#include "LudeoLog.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLudeo, Log, All);

typedef void (*LogCallbackType)(const struct FLudeoLogMessage&);

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Log Message"))
struct FLudeoLogMessage
{
	GENERATED_BODY()

	ELogVerbosity::Type LogLevel;

	FLudeoLogCategory LogCategory;

	TStringView<ANSICHAR> Timestamp;

	TStringView<ANSICHAR> Message;
};
