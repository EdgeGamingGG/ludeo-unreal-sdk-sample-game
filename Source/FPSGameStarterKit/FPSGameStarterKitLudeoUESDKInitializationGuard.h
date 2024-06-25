#pragma once

#include "FPSGameStarterKitLudeoGameInstance.h"

struct FFPSGameStarterKitLudeoUESDKInitializationGuard
{
public:
	static FFPSGameStarterKitLudeoUESDKInitializationGuard& GetInstance();

	int32 Initialize(FLudeoManager& LudeoManager);
	void Finalize(FLudeoManager& LudeoManager);

private:
	FFPSGameStarterKitLudeoUESDKInitializationGuard();
	~FFPSGameStarterKitLudeoUESDKInitializationGuard();

private:
	uint32 NumberOfInstanceInitialized;
};
