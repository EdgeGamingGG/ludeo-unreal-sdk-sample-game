#include "Blueprint/LudeoManager/LudeoManagerBlueprintFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

#include "LudeoManager/LudeoManager.h"

FLudeoResult ULudeoManagerBlueprintFunctionLibrary::InitializeLudeoSDK(UObject* WorldContextObject)
{
	FLudeoResult Result = LudeoResult::Unknown;

	if (const TSharedPtr<FLudeoManager> LudeoManager = FLudeoManager::GetInstance().Pin())
	{
		const FLudeoInitializationParameters InitializationParameters{};

		Result = LudeoManager->Initialize(InitializationParameters);
	}

	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString
		(
			WorldContextObject,
			TEXT("[Ludeo Session] Ludeo SDK initialized successfully"),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			WorldContextObject,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to initialize Ludeo SDK, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);
	}

	return Result;
}

void ULudeoManagerBlueprintFunctionLibrary::FinalizeLudeoSDK(UObject* WorldContextObject)
{
	FLudeoResult Result = LudeoResult::Unknown;

	if (const TSharedPtr<FLudeoManager> LudeoManager = FLudeoManager::GetInstance().Pin())
	{
		Result = LudeoManager->Finalize();
	}

	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString
		(
			WorldContextObject,
			TEXT("Ludeo SDK finalized successfully"),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			WorldContextObject,
			*FString::Printf
			(
				TEXT("Failed to finalize Ludeo SDK, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);
	}
}

void ULudeoManagerBlueprintFunctionLibrary::TickLudeoSDK()
{
	if (const TSharedPtr<FLudeoManager> LudeoManager = FLudeoManager::GetInstance().Pin())
	{
		LudeoManager->Tick();
	}
}
