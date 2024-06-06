#include "LudeoUESDK/LudeoUESDKModule.h"

void FLudeoUESDKModule::StartupModule()
{
	Super::StartupModule();

	LudeoManager = MakeShared<FLudeoManager>();
}

void FLudeoUESDKModule::ShutdownModule()
{
	Super::ShutdownModule();
}

IMPLEMENT_MODULE(FLudeoUESDKModule, LudeoUESDK);
