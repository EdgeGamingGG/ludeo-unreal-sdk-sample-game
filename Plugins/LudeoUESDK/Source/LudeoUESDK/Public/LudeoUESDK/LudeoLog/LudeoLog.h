#pragma once

#include "LudeoLogTypes.h"

#include <Ludeo/Init.h>

DECLARE_LOG_CATEGORY_EXTERN(LogLudeo, Log, All);

typedef void (*LogCallbackType)(const struct FLudeoLogMessage&);

struct FLudeoLogMessage
{
	ELogVerbosity::Type LogLevel;

	FLudeoLogCategory LogCategory;

	TStringView<ANSICHAR> Timestamp;

	TStringView<ANSICHAR> Message;
};
