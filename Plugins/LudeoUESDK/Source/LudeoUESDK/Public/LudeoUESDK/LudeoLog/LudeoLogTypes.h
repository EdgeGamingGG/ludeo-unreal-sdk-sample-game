#pragma once

#include "CoreMinimal.h"

#include <Ludeo/Utils.h>

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
