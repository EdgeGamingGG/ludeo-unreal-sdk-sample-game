#pragma once

#include <Ludeo/Init.h>

struct FLudeoInitializationParameters
{
	FLudeoInitializationParameters() :
		AllocateMemoryFunction(nullptr),
		ReAllocateMemoryFunction(nullptr),
		FreeMemoryFunction(nullptr)
	{

	}

	LudeoMemoryAllocFunc	AllocateMemoryFunction;
	LudeoMemoryReallocFunc	ReAllocateMemoryFunction;
	LudeoMemoryFreeFunc		FreeMemoryFunction;
};

struct FLudeoSetLoggingToFileParameters
{
	FLudeoSetLoggingToFileParameters() :
		bEnable(false)
	{

	}

	FString Directory;

	bool bEnable;
};
