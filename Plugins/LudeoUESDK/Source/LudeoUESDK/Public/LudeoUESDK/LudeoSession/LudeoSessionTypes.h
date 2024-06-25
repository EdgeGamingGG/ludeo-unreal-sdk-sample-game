#pragma once

#include "CoreMinimal.h"

#include <Ludeo/SessionTypes.h>

#include "LudeoUESDK/Ludeo/LudeoTypes.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoomTypes.h"
#include "LudeoUESDK/LudeoRoom/LudeoRoomWriterTypes.h"
#include "LudeoUESDK/LudeoResult.h"

#include "LudeoSessionTypes.generated.h"

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Handle"))
struct FLudeoSessionHandle
{
	GENERATED_BODY()

public:
	FORCEINLINE FLudeoSessionHandle(const LudeoHSession InSessionHandle = nullptr) :
		SessionHandle(static_cast<uint64>(reinterpret_cast<UPTRINT>(InSessionHandle)))
	{

	}

	FORCEINLINE operator LudeoHSession() const
	{
		return reinterpret_cast<LudeoHSession>(static_cast<UPTRINT>(SessionHandle));
	}

private:
	UPROPERTY(Transient)
	uint64 SessionHandle;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams
(
	FOnLudeoSessionDestroyedDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoSessionOnOpenRoomDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle,
	const FLudeoRoomHandle&, RoomHandle
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoSessionOnCloseRoomDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle,
	const FLudeoRoomHandle&, RoomHandle
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoSessionOnGetLudeoDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle,
	const FLudeoHandle&, LudeoHandle
);

DECLARE_DELEGATE_ThreeParams
(
	FLudeoSessionOnGetLudeoDelegate,
	const FLudeoResult&,
	const FLudeoSessionHandle&,
	const FLudeoHandle&
);

DECLARE_MULTICAST_DELEGATE_TwoParams
(
	FLudeoSessionOnLudeoSelectedMulticastDelegate,
	const FLudeoSessionHandle&,
	const FString&
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams
(
	FLudeoSessionNotificationDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoSessionOnLudeoSelectedDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle,
	const FString&, LudeoID
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoSessionOnRoomReadyDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle,
	const FLudeoRoomHandle&, RoomHandle
);

DECLARE_MULTICAST_DELEGATE_OneParam
(
	FLudeoSessionOnPauseGameRequestedMulticastDelegate,
	const FLudeoSessionHandle&
);

DECLARE_MULTICAST_DELEGATE_OneParam
(
	FLudeoSessionOnResumeGameRequestedMulticastDelegate,
	const FLudeoSessionHandle&
);

DECLARE_MULTICAST_DELEGATE_OneParam
(
	FLudeoSessionOnGameBackToMenuRequestedMulticastDelegate,
	const FLudeoSessionHandle&
);

DECLARE_MULTICAST_DELEGATE_TwoParams
(
	FLudeoSessionOnRoomReadyMulticastDelegate,
	const FLudeoSessionHandle&,
	const FLudeoRoomHandle&
);

DECLARE_DELEGATE_ThreeParams
(
	FLudeoSessionOnActivatedDelegate,
	const FLudeoResult&,
	const FLudeoSessionHandle&,
	const bool
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams
(
	FLudeoSessionOnActivatedDynamicMulticastDelegate,
	const FLudeoResult&, Result,
	const FLudeoSessionHandle&, SessionHandle,
	const bool, bIsLudeoSelected
);

DECLARE_DELEGATE_TwoParams
(
	FOnLudeoSessionDestroyedDelegate,
	const FLudeoResult&,
	const FLudeoSessionHandle&
);

DECLARE_MULTICAST_DELEGATE_TwoParams
(
	FOnLudeoSessionDestroyedMulticastDelegate,
	const FLudeoResult&,
	const FLudeoSessionHandle&
);

DECLARE_DELEGATE_TwoParams
(
	FLudeoSessionOnActivateSessionDelegate,
	const FLudeoResult&,
	const FLudeoSessionHandle&
);

DECLARE_DELEGATE_ThreeParams
(
	FLudeoSessionOnOpenRoomDelegate,
	const FLudeoResult&,
	const FLudeoSessionHandle&,
	const FLudeoRoomHandle&
);

DECLARE_DELEGATE_ThreeParams
(
	FLudeoSessionOnCloseRoomDelegate,
	const FLudeoResult&,
	const FLudeoSessionHandle&,
	const FLudeoRoomHandle&
);

USTRUCT(BlueprintType, meta = (DisplayName = "Create Ludeo Session Parameters"))
struct FCreateLudeoSessionParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Destroy Ludeo Session Parameters"))
struct FDestroyLudeoSessionParameters
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite)
	FLudeoSessionHandle SessionHandle;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session On Ludeo Selected Data"))
struct FLudeoSessionOnLudeoSelectedData
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadOnly)
	FLudeoSessionHandle SessionHandle;

	UPROPERTY(Transient, BlueprintReadOnly)
	FString LudeoID;

	UPROPERTY(Transient, BlueprintReadOnly)
	FLudeoHandle LudeoHandle;
};

UENUM(BlueprintType)
enum class ELudeoSessionAuthenticationType : uint8
{
	Steam
};
Expose_TNameOf(ELudeoSessionAuthenticationType);

USTRUCT(BlueprintType, meta = (DisplayName = "Activate Ludeo Session Steam Authentication Details"))
struct FLudeoSessionSteamAuthenticationDetails
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString AuthenticationID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString BetaBranchName;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Game Window Handle"))
struct FLudeoGameWindowHandle
{
	GENERATED_BODY()

	FLudeoGameWindowHandle(void* InNativeWindowHandle = nullptr) :
		NativeWindowHandle(InNativeWindowHandle)
	{

	}

	FORCEINLINE operator void*() const
	{
		return NativeWindowHandle;
	}

	static FLudeoGameWindowHandle GetGameWindowHandleFromWorld(const UObject* WorldContextObject)
	{
		if (WorldContextObject != nullptr)
		{
			UWorld* World = WorldContextObject->GetWorld();
			check(World != nullptr);

			if (UGameViewportClient* GameViewport = World->GetGameViewport())
			{
				if (const TSharedPtr<SWindow> Window = GameViewport->GetWindow())
				{
					if (const TSharedPtr<FGenericWindow> NativeWindow = Window->GetNativeWindow())
					{
						return NativeWindow->GetOSWindowHandle();
					}
				}
			}
		}

		return static_cast<void*>(nullptr);
	}

private:
	void* NativeWindowHandle;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Activate Session Parameters"))
struct FLudeoSessionActivateSessionParameters
{
	GENERATED_BODY()

	FLudeoSessionActivateSessionParameters() :
		AuthenticationType(ELudeoSessionAuthenticationType::Steam),
		bResetAttributeAndAction(false)
	{

	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ApiKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString AppToken;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	ELudeoSessionAuthenticationType AuthenticationType;

	// This is optional
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FLudeoSessionSteamAuthenticationDetails SteamAuthenticationDetails;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bResetAttributeAndAction;

	UPROPERTY(Transient, BlueprintReadWrite)
	FLudeoGameWindowHandle GameWindowHandle;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Get Ludeo Parameters"))
struct FLudeoSessionGetLudeoParameters
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString LudeoID;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Open Room Parameters"))
struct FLudeoSessionOpenRoomParameters
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString RoomID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString LudeoID;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Close Room Parameters"))
struct FLudeoSessionCloseRoomParameters
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite)
	FLudeoRoomHandle RoomHandle;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Open Gallery Parameters"))
struct FLudeoSessionOpenGalleryParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Subscribe To on Ludeo Selected Notification Parameters"))
struct FLudeoSessionSubscribeToOnLudeoSelectedNotificationParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Subscribe to on Pause Game Requested Notification Parameters"))
struct FLudeoSessionSubscribeToOnPauseGameRequestedNotificationParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Subscribe to on Resume Game Requested Notification Parameters"))
struct FLudeoSessionSubscribeToOnResumeGameRequestedNotificationParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Subscribe to on Game Back to Main Menu Requested Notification Parameters"))
struct FLudeoSessionSubscribeToOnGameBackToMainMenuRequestedNotificationParameters
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType, meta = (DisplayName = "Ludeo Session Subscribe to on Room Ready Notification Parameters"))
struct FLudeoSessionSubscribeToOnRoomReadyNotificationParameters
{
	GENERATED_BODY()
};
