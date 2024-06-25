#include "FPSGameStarterKitLudeoUESDKInitializationGuard.h"

#include "LudeoUESDK/LudeoManager/LudeoManager.h"

FFPSGameStarterKitLudeoUESDKInitializationGuard::FFPSGameStarterKitLudeoUESDKInitializationGuard() :
	NumberOfInstanceInitialized(0)
{
	TArray<FString> LudeoCommandPairCollection;
	{
		FString LudeoCommandList;
		FParse::Value(FCommandLine::Get(), TEXT("LudeoCommandList="), LudeoCommandList);

		LudeoCommandList.ParseIntoArray(LudeoCommandPairCollection, TEXT(","));
	}

	for (const FString& LudeoCommandPair : LudeoCommandPairCollection)
	{
		FString Key;
		FString Value;

		if (LudeoCommandPair.Split(TEXT(":"), &Key, &Value))
		{
			FLudeoManager::ExecuteLudeoCommand(*Key, *Value);
		}
	}
}

FFPSGameStarterKitLudeoUESDKInitializationGuard::~FFPSGameStarterKitLudeoUESDKInitializationGuard()
{

}

FFPSGameStarterKitLudeoUESDKInitializationGuard& FFPSGameStarterKitLudeoUESDKInitializationGuard::GetInstance()
{
	static FFPSGameStarterKitLudeoUESDKInitializationGuard InitializationGuard;
	return InitializationGuard;
}

int32 FFPSGameStarterKitLudeoUESDKInitializationGuard::Initialize(FLudeoManager& LudeoManager)
{
	if(NumberOfInstanceInitialized == 0)
	{
		LudeoManager.Initialize({});
	}

	NumberOfInstanceInitialized = NumberOfInstanceInitialized + 1;

	return NumberOfInstanceInitialized;
}

void FFPSGameStarterKitLudeoUESDKInitializationGuard::Finalize(FLudeoManager& LudeoManager)
{
	NumberOfInstanceInitialized = NumberOfInstanceInitialized - 1;

	if(NumberOfInstanceInitialized == 0)
	{
		LudeoManager.Finalize();
	}
}
