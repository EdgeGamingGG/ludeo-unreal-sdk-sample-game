#include "LudeoGameInstance.h"

#include "Engine/LevelScriptActor.h"
#include "Kismet/GameplayStatics.h"

#include "LudeoUESDK/LudeoScopedGuard.h"

#include "LudeoUESDKInitializationGuard.h"
#include "LudeoGameState.h"

ULudeoGameInstance::ULudeoGameInstance() :
	GameInstanceRegistrationID(INDEX_NONE)
{

}

void ULudeoGameInstance::Init()
{
	Super::Init();

	WeakLudeoManager = FLudeoManager::GetInstance();

	const TSharedPtr<FLudeoManager> LudeoManager = WeakLudeoManager.Pin();
	check(LudeoManager != nullptr);

	GameInstanceRegistrationID = FLudeoUESDKInitializationGuard::GetInstance().Initialize(*LudeoManager);
	
	LudeoManager->SetLogLevel(LudeoLogCategory::All, ELogVerbosity::Verbose);
	LudeoManager->SetLogLevel(LudeoLogCategory::Http, ELogVerbosity::Log);
	LudeoManager->SetLogLevel(LudeoLogCategory::Data, ELogVerbosity::Warning);
	LudeoManager->SetLogLevel(LudeoLogCategory::Coherent, ELogVerbosity::Log);

	// Register delegate for ticker callback
	TickDelegateHandle = FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ULudeoGameInstance::NativeTick));
}

void ULudeoGameInstance::Shutdown()
{
	const TSharedPtr<FLudeoManager> LudeoManager = WeakLudeoManager.Pin();
	check(LudeoManager != nullptr);

	// Remove delegate for ticker callback
	FTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

	DestoryLudeoSession();

	FLudeoUESDKInitializationGuard::GetInstance().Finalize(*LudeoManager);

	Super::Shutdown();
}

bool ULudeoGameInstance::OpenLudeoGallery(const FLudeoSessionHandle& LudeoSessionHandle)
{
	if (FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(LudeoSessionHandle))
	{
		const FLudeoResult Result = Session->OpenGallery();

		return Result.IsSuccessful();
	}

	return false;
}

bool ULudeoGameInstance::ExecuteLudeoCommand(const FString& Key, const FString& Value)
{
	return FLudeoManager::ExecuteLudeoCommand(*Key, *Value);
}

FLudeoSessionHandle ULudeoGameInstance::GetActiveLudeoSessionHandle(const UObject* WorldContextObject)
{
	check(WorldContextObject != nullptr);

	UWorld* World = WorldContextObject->GetWorld();
	check(World != nullptr);

	ULudeoGameInstance* GameInstance = World->GetGameInstance<ULudeoGameInstance>();
	check(GameInstance != nullptr);

	return GameInstance->LudeoSessionHandle.Get(nullptr);
}

bool ULudeoGameInstance::LoadLudeo(const FLudeoHandle& LudeoHandle)
{
	check(LudeoSessionHandle.IsSet());

	if(const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(LudeoHandle))
	{
		OnGetLudeo(LudeoResult::Success, LudeoSessionHandle.GetValue(), *Ludeo);

		return (PendingLudeoHandle == LudeoHandle);
	}

	return false;
}

void ULudeoGameInstance::LoadMainMenu()
{
	if (MainMenuPath.IsValid())
	{
		UGameplayStatics::OpenLevel(this, *MainMenuPath.ToString());
	}
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
	const bool bIsLudeoSelected,
	const FLudeoSessionOnActivatedDelegate OnSessionActivatedDelegate
)
{
	if (Result.IsSuccessful())
	{
		LudeoSessionHandle = SessionHandle;
	}
	else
	{
		LudeoSessionHandle.Reset();
	}

	OnSessionActivatedDelegate.ExecuteIfBound(Result, SessionHandle, bIsLudeoSelected);
}

FString ULudeoGameInstance::GetLudeoMapName(const FLudeo& Ludeo) const
{
	FString MapName;

	const TArray<FLudeoObjectInformation>& ObjectInformationCollection = Ludeo.GetLudeoObjectInformationCollection();

	const int32 SearchStartPosition = TStringView<TCHAR>(TEXT("/Game/FPS_Game/")).Len();

	const TStringView<TCHAR> SearchString(TEXT("Maps"));

	const int32 LevelScriptActorIndex = ObjectInformationCollection.IndexOfByPredicate([&](const FLudeoObjectInformation& ObjectInformation)
	{
		if (ObjectInformation.ObjectType.Len() >= (SearchStartPosition + SearchString.Len()))
		{
			const TStringView<TCHAR> Substring(ObjectInformation.ObjectType.GetCharArray().GetData() + SearchStartPosition, SearchString.Len());

			return (Substring == SearchString);
		}

		return false;
	});
	check(ObjectInformationCollection.IsValidIndex(LevelScriptActorIndex));

	if (ObjectInformationCollection.IsValidIndex(LevelScriptActorIndex))
	{
		ObjectInformationCollection[LevelScriptActorIndex].ObjectType.Split
		(
			TEXT("."),
			&MapName,
			nullptr,
			ESearchCase::CaseSensitive,
			ESearchDir::FromEnd
		);
	}
		
	return MapName;
}

void ULudeoGameInstance::OnGetLudeo(const FLudeoResult& Result, const FLudeoSessionHandle& SessionHandle, const FLudeoHandle& LudeoHandle)
{
	if (Result.IsSuccessful())
	{
		if (const FLudeo* Ludeo = FLudeo::GetLudeoByLudeoHandle(LudeoHandle))
		{
			const FString MapName = GetLudeoMapName(*Ludeo);

			if (ensureAlways(!MapName.IsEmpty()))
			{
				MarkLudeoAsPending(*Ludeo);

				UGameplayStatics::OpenLevel(this, *MapName);
			}
		}
	}
}

void ULudeoGameInstance::MarkLudeoAsPending(const FLudeo& Ludeo)
{
	const FLudeoHandle& LudeoHandle = Ludeo;

	if (PendingLudeoHandle != LudeoHandle)
	{
		ReleasePendingLudeo();

		PendingLudeoHandle = LudeoHandle;
	}
}

bool ULudeoGameInstance::ReleasePendingLudeo()
{
	if(PendingLudeoHandle != nullptr)
	{
		FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(GetActiveLudeoSessionHandle(this));
		check(Session != nullptr);

		if (Session->ReleaseLudeo(PendingLudeoHandle))
		{
			PendingLudeoHandle = nullptr;

			return true;
		}
	}

	return (PendingLudeoHandle == nullptr);
}

void ULudeoGameInstance::OnLudeoSelected(const FLudeoSessionHandle& SessionHandle, const FString& LudeoID)
{
	FLudeoSession* Session = FLudeoSession::GetSessionBySessionHandle(SessionHandle);
	check(Session != nullptr);

	if (Session != nullptr)
	{
		FLudeoSessionGetLudeoParameters GetLudeoParameters;
		GetLudeoParameters.LudeoID = LudeoID;

		Session->GetLudeo
		(
			GetLudeoParameters,
			FLudeoSessionOnGetLudeoDelegate::CreateUObject(this, &ULudeoGameInstance::OnGetLudeo)
		);
	}
}

void ULudeoGameInstance::OnPauseGameRequested(const FLudeoSessionHandle&)
{
	UGameplayStatics::SetGamePaused(this, true);
}

void ULudeoGameInstance::OnResumeGameRequested(const FLudeoSessionHandle&)
{
	UGameplayStatics::SetGamePaused(this, false);
}

void ULudeoGameInstance::OnGameBackToMainMenuRequested(const FLudeoSessionHandle&)
{
	PendingLudeoHandle = nullptr;

	LoadMainMenu();
}

bool ULudeoGameInstance::SetupLudeoSession(const FLudeoSessionOnActivatedDelegate& OnSessionActivatedDelegate)
{
	if(!LudeoSessionHandle.IsSet())
	{
		WeakLudeoManager = FLudeoManager::GetInstance();
	
		const TSharedPtr<FLudeoManager> LudeoManager = WeakLudeoManager.Pin();
		check(LudeoManager != nullptr);;
	
		FLudeoSessionManager& SessionManager = LudeoManager->GetSessionManager();
	
		const FCreateLudeoSessionParameters CreateSessionParameters;
	
		if (FLudeoSession* Session = SessionManager.CreateSession(CreateSessionParameters))
		{
			Session->GetOnLudeoSelectedDelegate().AddUObject(this, &ULudeoGameInstance::OnLudeoSelected);
			Session->GetOnPauseGameRequestedDelegate().AddUObject(this, &ULudeoGameInstance::OnPauseGameRequested);
			Session->GetOnResumeGameRequestedDelegate().AddUObject(this, &ULudeoGameInstance::OnResumeGameRequested);
			Session->GetOnGameBackToMenuRequestedDelegate().AddUObject(this, &ULudeoGameInstance::OnGameBackToMainMenuRequested);

			FLudeoSessionActivateSessionParameters ActivateSessionParameters;
			{
				ActivateSessionParameters.GameWindowHandle = FLudeoGameWindowHandle::GetGameWindowHandleFromWorld(this);

				ActivateSessionParameters.ApiKey = []()
				{
					FString APIKey;

					FParse::Value(FCommandLine::Get(), TEXT("APIKey="), APIKey);

					if(APIKey.IsEmpty())
					{
						GConfig->GetString(TEXT("Ludeo.SessionActivate"), TEXT("APIKey"), APIKey, GGameIni);
					}

					return APIKey;
				}();

				ActivateSessionParameters.AuthenticationType = [&]()
				{
					static UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, TNameOf<ELudeoSessionAuthenticationType>::GetName(), true);

					if (Enum != nullptr)
					{
						FString EnumString;
						FParse::Value(FCommandLine::Get(), TEXT("AuthenticationType="), EnumString);

						if(EnumString.IsEmpty())
						{
							GConfig->GetString(TEXT("Ludeo.SessionActivate"), TEXT("AuthenticationType"), EnumString, GGameIni);
						}

						if(!EnumString.IsEmpty())
						{
							return static_cast<ELudeoSessionAuthenticationType>(Enum->GetValueByNameString(EnumString));
						}
					}

					return ELudeoSessionAuthenticationType::Steam;
				}();
		
				if(ActivateSessionParameters.AuthenticationType == ELudeoSessionAuthenticationType::Steam)
				{
					FString& AuthenticationID = ActivateSessionParameters.SteamAuthenticationDetails.AuthenticationID;
					FString& DisplayName = ActivateSessionParameters.SteamAuthenticationDetails.DisplayName;

					if(IsDedicatedServerInstance())
					{
						FParse::Value(FCommandLine::Get(), TEXT("DedicatedServerAuthenticationID="), AuthenticationID);
						FParse::Value(FCommandLine::Get(), TEXT("DedicatedServerDisplayName="), DisplayName);

						if (AuthenticationID.IsEmpty() || DisplayName.IsEmpty())
						{
							GConfig->GetString
							(
								TEXT("Ludeo.SessionActivate"),
								TEXT("DedicatedServerAuthenticationID"),
								AuthenticationID,
								GGameIni
							);
						
							GConfig->GetString
							(
								TEXT("Ludeo.SessionActivate"),
								TEXT("DedicatedServerDisplayName"),
								ActivateSessionParameters.SteamAuthenticationDetails.DisplayName,
								GGameIni
							);
						}
					}
					else
					{
						const FString AuthenticationIDKey = FString::Printf(TEXT("AuthenticationID_%d"), GameInstanceRegistrationID);
						const FString DisplayNameKey = FString::Printf(TEXT("DisplayName_%d"), GameInstanceRegistrationID);

						FParse::Value(FCommandLine::Get(), *AuthenticationIDKey, AuthenticationID);
						FParse::Value(FCommandLine::Get(), *DisplayNameKey, DisplayName);

						if (AuthenticationID.IsEmpty() || DisplayName.IsEmpty())
						{
							GConfig->GetString
							(
								TEXT("Ludeo.SessionActivate"),
								*AuthenticationIDKey,
								ActivateSessionParameters.SteamAuthenticationDetails.AuthenticationID,
								GGameIni
							);	

							GConfig->GetString
							(
								TEXT("Ludeo.SessionActivate"),
								*DisplayNameKey,
								ActivateSessionParameters.SteamAuthenticationDetails.DisplayName,
								GGameIni
							);
						}
					}
				}
			}

			Session->Activate
			(
				ActivateSessionParameters,
				FLudeoSessionOnActivatedDelegate::CreateUObject
				(
					this,
					&ULudeoGameInstance::OnLudeoSessionActivated,
					OnSessionActivatedDelegate
				)
			);
	
			LudeoSessionHandle = nullptr;

			return true;
		}
	}
	
	return false;
}

void ULudeoGameInstance::DestoryLudeoSession(const FOnLudeoSessionDestroyedDelegate& OnSessionDestroyedDelegate)
{
	if (LudeoSessionHandle.IsSet())
	{
		if (FLudeoSessionManager* SessionManager = FLudeoSessionManager::GetInstance())
		{
			FDestroyLudeoSessionParameters DestroySessionParamters;
			DestroySessionParamters.SessionHandle = LudeoSessionHandle.GetValue();

			SessionManager->DestroySession(DestroySessionParamters, OnSessionDestroyedDelegate);
		}

		LudeoSessionHandle.Reset();
	}
}
