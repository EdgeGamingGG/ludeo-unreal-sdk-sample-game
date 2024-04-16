#pragma once

#include "CoreMinimal.h"

#include <Ludeo/SessionTypes.h>

#include "Ludeo/LudeoTypes.h"
#include "LudeoRoom/LudeoRoomTypes.h"
#include "LudeoRoom/LudeoRoomWriterTypes.h"

#include "LudeoResult.h"

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

USTRUCT(BlueprintType, meta = (DisplayName = "Activate Ludeo Session Parameters"))
struct FLudeoSessionActivateSessionParameters
{
	GENERATED_BODY()

	FLudeoSessionActivateSessionParameters() :
		bResetAttributeAndAction(false),
		GameWindowHandle(nullptr)
	{

	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString ApiKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString AppToken;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bResetAttributeAndAction;

	void* GameWindowHandle;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Open Room Parameters"))
struct FLudeoSessionOpenRoomParameters
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString RoomID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString LudeoID;
};

USTRUCT(BlueprintType, meta = (DisplayName = "Close Room Parameters"))
struct FLudeoSessionCloseRoomParameters
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadWrite)
	FLudeoRoomHandle RoomHandle;
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
