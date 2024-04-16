#pragma once

#include "Kismet/BlueprintAsyncActionBase.h"

#include "LudeoPlayer/LudeoPlayerTypes.h"

#include "LudeoPlayerBlueprintAsyncAction.generated.h"

UCLASS()
class LUDEOUESDK_API ULudeoPlayerBeginGameplayAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoPlayerOnBeginGameplayDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoPlayerOnBeginGameplayDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Player",
			DisplayName = "Ludeo Player Begin Gameplay",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoPlayerBeginGameplayAsyncNode* LudeoPlayerBeginGameplay
	(
		UObject* WorldContextObject,
		const FLudeoPlayerHandle& InPlayerHandle,
		const FLudeoPlayerBeginGameplayParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnBeginGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle);

	void OnResultReady(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle);

private:
	FLudeoPlayerHandle PlayerHandle;
	FLudeoPlayerBeginGameplayParameters Parameters;
};

UCLASS()
class LUDEOUESDK_API ULudeoPlayerEndGameplayAsyncNode : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Success"))
	FLudeoPlayerOnEndGameplayDynamicMulticastDelegate OnSuccessDelegate;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "On Fail"))
	FLudeoPlayerOnEndGameplayDynamicMulticastDelegate OnFailDelegate;

	UFUNCTION
	(
		BlueprintCallable,
		meta =
		(
			Category = "Ludeo Session",
			DisplayName = "Ludeo Session End Gameplay",
			BlueprintInternalUseOnly = "true",
			DefaultToSelf = "WorldContextObject",
			HidePin = "WorldContextObject"
		)
	)
	static ULudeoPlayerEndGameplayAsyncNode* LudeoPlayerEndGameplay
	(
		UObject* WorldContextObject,
		const FLudeoPlayerHandle& InPlayerHandle,
		const FLudeoPlayerEndGameplayParameters& InParameters
	);

	virtual void Activate() override;

private:
	void OnEndGameplay(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle);

	void OnResultReady(const FLudeoResult& Result, const FLudeoPlayerHandle& InPlayerHandle);

private:
	FLudeoPlayerHandle PlayerHandle;
	FLudeoPlayerEndGameplayParameters Parameters;
};
