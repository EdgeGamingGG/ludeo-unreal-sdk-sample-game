#pragma once

#include "CoreMinimal.h"

#if 0 && WITH_EDITOR
	#define LUDEO_FNAME_TO_UTF8(Name) TCHAR_TO_UTF8(*Name.ToString())
#else
	#define LUDEO_FNAME_TO_UTF8(Name) FLudeoUtility::FastConvertAttributeFNameToCString(Name)
#endif

class FLudeoUtility
{
public:
	static const char* FastConvertAttributeFNameToCString(const FName& Name);
};
