#include "LudeoGameInstance.h"

ULudeoGameInstance::ULudeoGameInstance() :
	LudeoSessionHandle(nullptr)
{

}

void ULudeoGameInstance::Init()
{
	Super::Init();

	WeakLudeoManager = FLudeoManager::GetInstance();

	const TSharedPtr<FLudeoManager> LudeoManager = WeakLudeoManager.Pin();
	check(LudeoManager != nullptr);

	const FLudeoResult Result = LudeoManager->Initialize({});
	check(Result.IsSuccessful());

	// Register delegate for ticker callback
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ULudeoGameInstance::NativeTick));
}

void ULudeoGameInstance::Shutdown()
{
	// Remove delegate for ticker callback
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	if (const TSharedPtr<FLudeoManager> LudeoManager = WeakLudeoManager.Pin())
	{
		LudeoManager->Finalize();
	}

	Super::Shutdown();
}

bool ULudeoGameInstance::NativeTick(float DeltaSeconds)
{
	const TSharedPtr<FLudeoManager> LudeoManager = WeakLudeoManager.Pin();
	check(LudeoManager != nullptr);

	LudeoManager->Tick();

	Tick(DeltaSeconds);
	
	return true;
}


void ULudeoGameInstance::OnLudeoSessionActivated
(
	const FLudeoResult& Result,
	const FLudeoSessionHandle& SessionHandle,
	const bool bIsLudeoSelected
)
{
	if (Result.IsSuccessful())
	{
		LudeoSessionHandle = SessionHandle;
	}
}

void ULudeoGameInstance::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle)
{
	if (Result.IsSuccessful())
	{
		LudeoSessionHandle = nullptr;
	}
}

bool ULudeoGameInstance::SetupLudeoSession
(
	const FLudeoSessionOnActivatedDelegate& OnSessionActivatedDelegate,
	const FLudeoSessionOnLudeoSelectedMulticastDelegate::FDelegate& OnLudeoSelectedDelegate
)
{
	WeakLudeoManager = FLudeoManager::GetInstance();

	const TSharedPtr<FLudeoManager> LudeoManager = WeakLudeoManager.Pin();
	check(LudeoManager != nullptr);;

	FLudeoSessionManager& SessionManager = LudeoManager->GetSessionManager();

	const FCreateLudeoSessionParameters CreateSessionParameters;

	if (FLudeoSession* Session = SessionManager.CreateSession(CreateSessionParameters))
	{
		LudeoSessionHandle = *Session;

		Session->GetOnLudeoSelectedDelegate().Add(OnLudeoSelectedDelegate);

		FLudeoSessionActivateSessionParameters ActivateSessionParameters;

		GConfig->GetString(TEXT("Ludeo.SessionActivate"), TEXT("APIKey"), ActivateSessionParameters.ApiKey, GGameIni);
		GConfig->GetBool(TEXT("Ludeo.SessionActivate"), TEXT("bResetAttributeAndAction"), ActivateSessionParameters.bResetAttributeAndAction, GGameIni);

		ActivateSessionParameters.GameWindowHandle = []()
		{
			if (GEngine != nullptr && GEngine->GameViewport != nullptr)
			{
				if (const TSharedPtr<SWindow> Window = GEngine->GameViewport->GetWindow())
				{
					if (const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow())
					{
						return NativeWindow->GetOSWindowHandle();
					}
				}
			}

			return static_cast<void*>(nullptr);
		}();

		Session->Activate
		(
			ActivateSessionParameters,
			FLudeoSessionOnActivatedDelegate::CreateWeakLambda
			(
				this,
				[this, OnSessionActivatedDelegate]
				(
					const FLudeoResult& Result,
					const FLudeoSessionHandle& SessionHandle,
					const bool bIsLudeoSelected
				)
				{
					OnLudeoSessionActivated(Result, SessionHandle, bIsLudeoSelected);

					OnSessionActivatedDelegate.ExecuteIfBound(Result, SessionHandle, bIsLudeoSelected);
				}
			)
		);

		return true;
	}

	return false;
}

void ULudeoGameInstance::DestoryLudeoSession(const FOnLudeoSessionDestroyedDelegate& OnSessionDestroyedDelegate)
{
	if (LudeoSessionHandle != nullptr)
	{
		if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
		{
			FDestroyLudeoSessionParameters DestroySessionParamters;
			DestroySessionParamters.SessionHandle = LudeoSessionHandle;

			SessionManager->DestroySession(DestroySessionParamters, OnSessionDestroyedDelegate);
		}
	}
}
