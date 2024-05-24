#include "LudeoUESDK/Blueprint/LudeoPlayer/LudeoPlayerBlueprintAsyncAction.h"

#include "Kismet/KismetSystemLibrary.h"

#include "LudeoUESDK/LudeoPlayer/LudeoPlayer.h"

ULudeoPlayerBeginGameplayAsyncNode* ULudeoPlayerBeginGameplayAsyncNode::BeginGameplay
(
	UObject* WorldContextObject,
	const FLudeoPlayerHandle& InPlayerHandle,
	const FLudeoPlayerBeginGameplayParameters& InParameters
)
{
	ULudeoPlayerBeginGameplayAsyncNode* AsyncNode = NewObject<ULudeoPlayerBeginGameplayAsyncNode>(WorldContextObject);
	AsyncNode->PlayerHandle = InPlayerHandle;
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoPlayerBeginGameplayAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Beginning gameplay..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (const FLudeoPlayer* Player = FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle))
	{
		Player->BeginGameplay
		(
			Parameters,
			FLudeoPlayerOnBeginGameplayDelegate::CreateUObject(this, &ULudeoPlayerBeginGameplayAsyncNode::OnBeginGameplay)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, PlayerHandle);
	}
}

void ULudeoPlayerBeginGameplayAsyncNode::OnBeginGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle)
{
	OnResultReady(Result, InPlayerHandle);
}

void ULudeoPlayerBeginGameplayAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Player] Gameplay begun successfully"), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

		OnSuccessDelegate.Broadcast(Result, InPlayerHandle);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to begin gameplay, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InPlayerHandle);
	}
}

ULudeoPlayerEndGameplayAsyncNode* ULudeoPlayerEndGameplayAsyncNode::EndGameplay
(
	UObject* WorldContextObject,
	const FLudeoPlayerHandle& InPlayerHandle,
	const FLudeoPlayerEndGameplayParameters& InParameters
)
{
	ULudeoPlayerEndGameplayAsyncNode* AsyncNode = NewObject<ULudeoPlayerEndGameplayAsyncNode>(WorldContextObject);
	AsyncNode->PlayerHandle = InPlayerHandle;
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoPlayerEndGameplayAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Player] Ending gameplay..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (const FLudeoPlayer* Player = FLudeoPlayer::GetPlayerByPlayerHandle(PlayerHandle))
	{
		Player->EndGameplay
		(
			Parameters,
			FLudeoPlayerOnEndGameplayDelegate::CreateUObject(this, &ULudeoPlayerEndGameplayAsyncNode::OnEndGameplay)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, PlayerHandle);
	}
}

void ULudeoPlayerEndGameplayAsyncNode::OnEndGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle)
{
	OnResultReady(Result, InPlayerHandle);
}

void ULudeoPlayerEndGameplayAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Gameplay ended successfully"), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

		OnSuccessDelegate.Broadcast(Result, InPlayerHandle);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to end gameplay, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InPlayerHandle);
	}
}
