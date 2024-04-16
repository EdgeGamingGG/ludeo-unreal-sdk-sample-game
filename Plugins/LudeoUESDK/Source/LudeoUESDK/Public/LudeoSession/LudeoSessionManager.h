#pragma once

#include "CoreMinimal.h"

#include "LudeoSession/LudeoSession.h"

class LUDEOUESDK_API FLudeoSessionManager
{
	friend struct FLudeoSession;
	friend struct FLudeo;
	friend struct FLudeoRoom;
	friend struct FLudeoPlayer;
	friend struct FLudeoRoomWriter;

	friend class FLudeoObjectStateManager;
	friend class FLudeoManager;

private:
	void Finalize();

public:
	static FLudeoSessionManager* GetInstance();

	FLudeoSession* CreateSession(const FCreateLudeoSessionParameters& CreateLudeoSessionParameters = FCreateLudeoSessionParameters());
	void DestroySession
	(
		const FDestroyLudeoSessionParameters& DestroyLudeoSessionParameters,
		const FOnLudeoSessionDestroyedDelegate& OnLudeoSessionDestroyedDelegate = {}
	) const;

	FLudeoSession* GetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle) const;

private:
	static FLudeoSession* StaticGetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle);
	static FLudeoSession* StaticGetSessionByRoomHandle(const FLudeoRoomHandle& RoomHandle);
	static FLudeoSession* StaticGetSessionByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle);
	static FLudeoSession* StaticGetSessionByLudeoHandle(const FLudeoHandle& LudeoHandle);

private:
	static void StaticOnSessionDestroyed(const LudeoSessionReleaseCallbackParams* pSessionReleaseCallbackParams);
	void OnSessionDestroyed(const LudeoSessionReleaseCallbackParams& SessionReleaseCallbackParams);

private:
	TArray<FLudeoSession> SessionCollection;
};
