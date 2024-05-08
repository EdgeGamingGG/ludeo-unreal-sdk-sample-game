#include "LudeoUESDK/LudeoUESDKModule.h"

void FLudeoUESDKModule::StartupModule()
{
	Super::StartupModule();

	LudeoManager = MakeShared<FLudeoManager>();

#if WITH_DEV_AUTOMATION_TESTS
	FPlatformProcess::GetDllHandle(TEXT(LUDEOSDK_DLL_NAME));
#endif
}

void FLudeoUESDKModule::ShutdownModule()
{
	Super::ShutdownModule();
}

IMPLEMENT_MODULE(FLudeoUESDKModule, LudeoUESDK);
