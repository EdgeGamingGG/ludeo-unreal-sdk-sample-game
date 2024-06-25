#include "LudeoUESDK/Blueprint/LudeoSession/LudeoSessionBlueprintAsyncAction.h"

#include "Kismet/KismetSystemLibrary.h"

#include "LudeoUESDK/LudeoSession/LudeoSessionManager.h"

ULudeoDestroySessionAsyncNode* ULudeoDestroySessionAsyncNode::DestroyLudeoSession
(
	UObject* WorldContextObject,
	const FDestroyLudeoSessionParameters& InParameters
)
{
	ULudeoDestroySessionAsyncNode* AsyncNode = NewObject<ULudeoDestroySessionAsyncNode>(WorldContextObject);
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoDestroySessionAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Destroying session..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
	{
		SessionManager->DestroySession
		(
			Parameters,
			FOnLudeoSessionDestroyedDelegate::CreateUObject(this, &ULudeoDestroySessionAsyncNode::OnLudeoSessionDestroyed)
		);
	}
	else
	{
		OnResultReady(LudeoResult::SDKDisabled, nullptr);
	}
}

void ULudeoDestroySessionAsyncNode::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle)
{
	OnResultReady(Result, SessionHandle);
}

void ULudeoDestroySessionAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Session destroyed successfully"), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

		OnSuccessDelegate.Broadcast(Result, InSessionHandle);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to destroy session, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InSessionHandle);
	}
}

void ULudeoSessionSubscribeNotificationAsyncNodeBase::Activate()
{
	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetOnDestroySessionDelegate().AddUObject(this, &ULudeoSessionSubscribeNotificationAsyncNodeBase::OnLudeoSessionDestroyed);
	}
}

void ULudeoSessionSubscribeNotificationAsyncNodeBase::OnLudeoSessionDestroyed(const FLudeoResult&, const FLudeoSessionHandle&)
{

}

ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode* ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode::SubscribeOnLudeoSelectedNotification
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle
)
{
	ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode* AsyncNode = NewObject<ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;

	return AsyncNode;
}

void ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] Subscribing to on Ludeo selected notification..."),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0), 5.0f
	);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetOnLudeoSelectedDelegate().AddUObject(this, &ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode::OnLudeoSelected);

		OnSuccessDelegate.Broadcast(LudeoResult::Success, SessionHandle, FString());
	}
	else
	{
		OnFailDelegate.Broadcast(LudeoResult::InvalidParameters, SessionHandle, FString());
	}
}

void ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle)
{
	Super::OnLudeoSessionDestroyed(Result, InSessionHandle);

	if(Result.IsSuccessful())
	{
		OnFailDelegate.Broadcast(LudeoResult::Canceled, SessionHandle, FString());
	}
}

void ULudeoSessionSubscribeToOnLudeoSelectedNotificationAsyncNode::OnLudeoSelected
(
	const FLudeoSessionHandle& InSessionHandle,
	const FString& LudeoID
)
{
	UKismetSystemLibrary::PrintString
	(
		this,
		*FString::Printf
		(
			TEXT("[Ludeo Session] Ludeo selected, ID: %s"),
			*LudeoID
		),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0),
		5.0f
	);

	OnLudeoSelectedDelegate.Broadcast(LudeoResult::Success, InSessionHandle, LudeoID);
}

ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode* ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode::SubscribeToOnPauseGameRequestedNotification
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle
)
{
	ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode* AsyncNode = NewObject<ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;

	return AsyncNode;
}

void ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] Subscribing to on pause game requested notification..."),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0), 5.0f
	);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetOnPauseGameRequestedDelegate().AddUObject(this, &ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode::OnPauseGameRequested);

		OnSuccessDelegate.Broadcast(LudeoResult::Success, SessionHandle);
	}
	else
	{
		OnFailDelegate.Broadcast(LudeoResult::InvalidParameters, SessionHandle);
	}
}

void ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle)
{
	Super::OnLudeoSessionDestroyed(Result, InSessionHandle);

	if(Result.IsSuccessful())
	{
		OnFailDelegate.Broadcast(LudeoResult::Canceled, SessionHandle);
	}
}

void ULudeoSessionSubscribeToOnPauseGameRequestedNotificationAsyncNode::OnPauseGameRequested
(
	const FLudeoSessionHandle& InSessionHandle
)
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] On pause game requested"),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0),
		5.0f
	);

	OnPauseGameRequestedDelegate.Broadcast(LudeoResult::Success, InSessionHandle);
}

ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode* ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode::SubscribeToOnResumeGameRequestedNotification
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle
)
{
	ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode* AsyncNode = NewObject<ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;

	return AsyncNode;
}

void ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] Subscribing to on resume game requested notification..."),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0), 5.0f
	);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetOnResumeGameRequestedDelegate().AddUObject(this, &ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode::OnResumeGameRequested);

		OnSuccessDelegate.Broadcast(LudeoResult::Success, SessionHandle);
	}
	else
	{
		OnFailDelegate.Broadcast(LudeoResult::InvalidParameters, SessionHandle);
	}
}

void ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle)
{
	Super::OnLudeoSessionDestroyed(Result, InSessionHandle);

	if (Result.IsSuccessful())
	{
		OnFailDelegate.Broadcast(LudeoResult::Canceled, SessionHandle);
	}
}

void ULudeoSessionSubscribeToOnResumeGameRequestedNotificationAsyncNode::OnResumeGameRequested
(
	const FLudeoSessionHandle& InSessionHandle
)
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] On resume game requested"),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0),
		5.0f
	);

	OnResumeGameRequestedDelegate.Broadcast(LudeoResult::Success, InSessionHandle);
}

ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode* ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode::SubscribeToOnGameBackToMainMenuRequestedNotification
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle
)
{
	ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode* AsyncNode = NewObject<ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;

	return AsyncNode;
}

void ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] Subscribing to on game back to main menu requested notification..."),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0), 5.0f
	);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetOnGameBackToMenuRequestedDelegate().AddUObject(this, &ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode::OnGameBackToMainMenuRequested);

		OnSuccessDelegate.Broadcast(LudeoResult::Success, SessionHandle);
	}
	else
	{
		OnFailDelegate.Broadcast(LudeoResult::InvalidParameters, SessionHandle);
	}
}

void ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle)
{
	Super::OnLudeoSessionDestroyed(Result, InSessionHandle);

	if(Result.IsSuccessful())
	{
		OnFailDelegate.Broadcast(LudeoResult::Canceled, SessionHandle);
	}
}

void ULudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationAsyncNode::OnGameBackToMainMenuRequested
(
	const FLudeoSessionHandle& InSessionHandle
)
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] On game back to main menu notification"),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0),
		5.0f
	);

	OnGameBackToMainMenuRequestedDelegate.Broadcast(LudeoResult::Success, InSessionHandle);
}

ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode* ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode::SubscribeToOnRoomReadyNotification
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle
)
{
	ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode* AsyncNode = NewObject<ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;

	return AsyncNode;
}

void ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] Subscribing to on room ready notification..."),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0), 5.0f
	);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetOnRoomReadyDelegate().AddUObject(this, &ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode::OnRoomReady);

		OnSuccessDelegate.Broadcast(LudeoResult::Success, SessionHandle, nullptr);
	}
	else
	{
		OnFailDelegate.Broadcast(LudeoResult::InvalidParameters, SessionHandle, nullptr);
	}
}

void ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle)
{
	Super::OnLudeoSessionDestroyed(Result, InSessionHandle);

	if (Result.IsSuccessful())
	{
		OnFailDelegate.Broadcast(LudeoResult::Canceled, SessionHandle, nullptr);
	}
}

void ULudeoSessionSubscribeToOnRoomReadyNotificationAsyncNode::OnRoomReady
(
	const FLudeoSessionHandle& InSessionHandle,
	const FLudeoRoomHandle& InRoomHandle
)
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] On room ready notification"),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0),
		5.0f
	);

	OnRoomReadyDelegate.Broadcast(LudeoResult::Success, InSessionHandle, InRoomHandle);
}

ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode* ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode::SubscribeToOnPlayerConsentUpdatedNotification
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle
)
{
	ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode* AsyncNode = NewObject<ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;

	return AsyncNode;
}

void ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] Subscribing to on player consent updated notification..."),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0), 5.0f
	);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetOnPlayerConsentUpdatedDelegate().AddUObject(this, &ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode::OnPlayerConsentUpdated);

		OnSuccessDelegate.Broadcast(LudeoResult::Success, SessionHandle, {});
	}
	else
	{
		OnFailDelegate.Broadcast(LudeoResult::InvalidParameters, SessionHandle, {});
	}
}

void ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode::OnLudeoSessionDestroyed(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle)
{
	Super::OnLudeoSessionDestroyed(Result, InSessionHandle);

	if (Result.IsSuccessful())
	{
		OnFailDelegate.Broadcast(LudeoResult::Canceled, SessionHandle, {});
	}
}

void ULudeoSessionSubscribeToOnPlayerConsentUpdatedNotificationAsyncNode::OnPlayerConsentUpdated
(
	const FLudeoSessionHandle& InSessionHandle,
	const FLudeoSessionPlayerConsentData& InPlayerConsentData
)
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] On player consent updated notification"),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0),
		5.0f
	);

	OnPlayerConsentUpdatedDelegate.Broadcast(LudeoResult::Success, InSessionHandle, InPlayerConsentData);
}

ULudeoSessionActivateSessionAsyncNode* ULudeoSessionActivateSessionAsyncNode::ActivateSession
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle,
	const FLudeoSessionActivateSessionParameters& InParameters
)
{
	ULudeoSessionActivateSessionAsyncNode* AsyncNode = NewObject<ULudeoSessionActivateSessionAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;
	AsyncNode->Parameters = InParameters;

	if(AsyncNode->Parameters.GameWindowHandle == nullptr)
	{
		AsyncNode->Parameters.GameWindowHandle = []()
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
	}

	return AsyncNode;
}

void ULudeoSessionActivateSessionAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString
	(
		this,
		TEXT("[Ludeo Session] Activate Ludeo session..."),
		true,
		true,
		FLinearColor(0.0, 0.66, 1.0), 5.0f
	);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->Activate
		(
			Parameters,
			FLudeoSessionOnActivatedDelegate::CreateUObject(this, &ULudeoSessionActivateSessionAsyncNode::OnActivated)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, SessionHandle, false);
	}
}

void ULudeoSessionActivateSessionAsyncNode::OnActivated
(
	const FLudeoResult& Result,
	const FLudeoSessionHandle& InSessionHandle,
	const bool bIsLudeoSelected
)
{
	OnResultReady(Result, InSessionHandle, bIsLudeoSelected);
}

void ULudeoSessionActivateSessionAsyncNode::OnResultReady
(
	const FLudeoResult& Result,
	const FLudeoSessionHandle& InSessionHandle,
	const bool bIsLudeoSelected
)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Session activated successfully"), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

		OnSuccessDelegate.Broadcast(Result, InSessionHandle, bIsLudeoSelected);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to activate session, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InSessionHandle, bIsLudeoSelected);
	}

	SetReadyToDestroy();
}

/**/
ULudeoSessionGetLudeoAsyncNode* ULudeoSessionGetLudeoAsyncNode::GetLudeo
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle,
	const FLudeoSessionGetLudeoParameters& InParameters
)
{
	ULudeoSessionGetLudeoAsyncNode* AsyncNode = NewObject<ULudeoSessionGetLudeoAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoSessionGetLudeoAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Getting ludeo..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->GetLudeo
		(
			Parameters,
			FLudeoSessionOnGetLudeoDelegate::CreateUObject(this, &ULudeoSessionGetLudeoAsyncNode::OnGetLudeo)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, SessionHandle, nullptr);
	}
}

void ULudeoSessionGetLudeoAsyncNode::OnGetLudeo(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoHandle& LudeoHandle)
{
	OnResultReady(Result, InSessionHandle, LudeoHandle);
}

void ULudeoSessionGetLudeoAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoHandle& LudeoHandle)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Ludeo is retrieved successfully"), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

		OnSuccessDelegate.Broadcast(Result, InSessionHandle, LudeoHandle);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to get Ludeo, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InSessionHandle, LudeoHandle);
	}
}
/**/

ULudeoSessionOpenRoomAsyncNode* ULudeoSessionOpenRoomAsyncNode::OpenRoom
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle,
	const FLudeoSessionOpenRoomParameters& InParameters
)
{
	ULudeoSessionOpenRoomAsyncNode* AsyncNode = NewObject<ULudeoSessionOpenRoomAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoSessionOpenRoomAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Opening room..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->OpenRoom
		(
			Parameters,
			FLudeoSessionOnOpenRoomDelegate::CreateUObject(this, &ULudeoSessionOpenRoomAsyncNode::OnOpenRoom)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, SessionHandle, nullptr);
	}
}

void ULudeoSessionOpenRoomAsyncNode::OnOpenRoom(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle)
{
	OnResultReady(Result, InSessionHandle, InRoomHandle);
}

void ULudeoSessionOpenRoomAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Room opened successfully"), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

		OnSuccessDelegate.Broadcast(Result, InSessionHandle, InRoomHandle);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to open room, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InSessionHandle, InRoomHandle);
	}

	SetReadyToDestroy();
}

ULudeoSessionCloseRoomAsyncNode* ULudeoSessionCloseRoomAsyncNode::CloseRoom
(
	UObject* WorldContextObject,
	const FLudeoSessionHandle& InSessionHandle,
	const FLudeoSessionCloseRoomParameters& InParameters
)
{
	ULudeoSessionCloseRoomAsyncNode* AsyncNode = NewObject<ULudeoSessionCloseRoomAsyncNode>(WorldContextObject);
	AsyncNode->SessionHandle = InSessionHandle;
	AsyncNode->Parameters = InParameters;

	return AsyncNode;
}

void ULudeoSessionCloseRoomAsyncNode::Activate()
{
	UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Closing room..."), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

	Super::Activate();

	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle))
	{
		Session->CloseRoom
		(
			Parameters,
			FLudeoSessionOnCloseRoomDelegate::CreateUObject(this, &ULudeoSessionCloseRoomAsyncNode::OnCloseRoom)
		);
	}
	else
	{
		OnResultReady(LudeoResult::InvalidParameters, SessionHandle, nullptr);
	}
}

void ULudeoSessionCloseRoomAsyncNode::OnCloseRoom(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle)
{
	OnResultReady(Result, InSessionHandle, InRoomHandle);
}

void ULudeoSessionCloseRoomAsyncNode::OnResultReady(const FLudeoResult& Result, const FLudeoSessionHandle& InSessionHandle, const FLudeoRoomHandle& InRoomHandle)
{
	if (Result.IsSuccessful())
	{
		UKismetSystemLibrary::PrintString(this, TEXT("[Ludeo Session] Room closed successfully"), true, true, FLinearColor(0.0, 0.66, 1.0), 5.0f);

		OnSuccessDelegate.Broadcast(Result, InSessionHandle, InRoomHandle);
	}
	else
	{
		UKismetSystemLibrary::PrintString
		(
			this,
			*FString::Printf
			(
				TEXT("[Ludeo Session] Failed to close room, reason: %s"),
				UTF8_TO_TCHAR(Result.ToString().GetData())
			),
			true,
			true,
			FLinearColor(0.0, 0.66, 1.0),
			5.0f
		);

		OnFailDelegate.Broadcast(Result, InSessionHandle, InRoomHandle);
	}

	SetReadyToDestroy();
}
