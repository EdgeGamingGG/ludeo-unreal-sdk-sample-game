#include "LudeoUESDKInitializationGuard.h"

FLudeoUESDKInitializationGuard::FLudeoUESDKInitializationGuard() :
	NumberOfInstanceInitialized(0)
{

}

FLudeoUESDKInitializationGuard::~FLudeoUESDKInitializationGuard()
{

}

FLudeoUESDKInitializationGuard& FLudeoUESDKInitializationGuard::GetInstance()
{
	static FLudeoUESDKInitializationGuard InitializationGuard;
	return InitializationGuard;
}

int32 FLudeoUESDKInitializationGuard::Initialize(FLudeoManager& LudeoManager)
{
	if(NumberOfInstanceInitialized == 0)
	{
		LudeoManager.Initialize({});
	}

	NumberOfInstanceInitialized = NumberOfInstanceInitialized + 1;

	return NumberOfInstanceInitialized;
}

void FLudeoUESDKInitializationGuard::Finalize(FLudeoManager& LudeoManager)
{
	NumberOfInstanceInitialized = NumberOfInstanceInitialized - 1;

	if(NumberOfInstanceInitialized == 0)
	{
		LudeoManager.Finalize();
	}
}
