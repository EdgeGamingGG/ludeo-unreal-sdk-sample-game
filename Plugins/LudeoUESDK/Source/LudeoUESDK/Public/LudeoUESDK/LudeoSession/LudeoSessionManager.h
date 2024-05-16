#pragma once

#include "CoreMinimal.h"

#include "LudeoUESDK/LudeoSession/LudeoSession.h"

class LUDEOUESDK_API FLudeoSessionManager
{
	friend class FLudeoSession;
	friend class FLudeo;
	friend class FLudeoRoom;
	friend class FLudeoPlayer;
	friend class FLudeoRoomWriter;

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
	);

	FLudeoSession* GetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle) const;

private:
	static FLudeoSession* StaticGetSessionBySessionHandle(const FLudeoSessionHandle& SessionHandle);
	static FLudeoSession* StaticGetSessionByRoomHandle(const FLudeoRoomHandle& RoomHandle);
	static FLudeoSession* StaticGetSessionByPlayerHandle(const FLudeoPlayerHandle& PlayerHandle);
	static FLudeoSession* StaticGetSessionByLudeoHandle(const FLudeoHandle& LudeoHandle);

private:
	TArray<FLudeoSession> SessionCollection;
};
