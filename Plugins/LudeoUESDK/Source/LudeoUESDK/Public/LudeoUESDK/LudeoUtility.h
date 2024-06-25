#pragma once

#include "CoreMinimal.h"

#if 1
	#define LUDEO_FNAME_TO_UTF8(Name) FLudeoUtility::FastConvertAttributeFNameToCString(Name)
#else
	#define LUDEO_FNAME_TO_UTF8(Name) TCHAR_TO_UTF8(*Name.ToString())
#endif

#define LUDEO_FSTRING_TO_UTF8(Name) TCHAR_TO_UTF8(*Name)

class FLudeoUtility
{
public:
	static const char* FastConvertAttributeFNameToCString(const FName& Name);
};
