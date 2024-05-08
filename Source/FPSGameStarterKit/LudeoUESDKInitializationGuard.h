#pragma once

#include "LudeoGameInstance.h"

struct FLudeoUESDKInitializationGuard
{
public:
	static FLudeoUESDKInitializationGuard& GetInstance();

	int32 Initialize(FLudeoManager& LudeoManager);
	void Finalize(FLudeoManager& LudeoManager);

private:
	FLudeoUESDKInitializationGuard();
	~FLudeoUESDKInitializationGuard();

private:
	uint32 NumberOfInstanceInitialized;
};
