// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/GameplaySession.h"

#pragma pack(push, 8)

/** Opaque Room handle */
EXTERN_C typedef struct LudeoRoomHandle* LudeoHRoom;

//
// ludeo_Room_GetInfo related types
//

/** The most recent version of ludeo_Room_GetInfo */
#define LUDEO_ROOM_GETINFO_API_LATEST 1

LUDEO_STRUCT(LudeoRoomGetInfoParams, (
	/** Set this to LUDEO_ROOM_GETINFO_API_LATEST */
	int32_t apiVersion;
));

/** Latest version of the LudeoRoomInfo struct */
#define LUDEO_ROOMINFO_API_LATEST 1
/** 
 * Information that can be retrieved with the ludeo_Room_GetInfo function
 * Any pointers in this struct point to the SDK's internal memory, and therefore the client should not keep hold of them.
 * As soon as the client gets the information, it should copy what it needs before making any other calls to the SDK.
 */
LUDEO_STRUCT(LudeoRoomInfo, (

	/**
	 * Set by the SDK when allocating the struct
	 */
	int32_t apiVersion;

	/**
	 * Room id.
	 * This is the id supplied by the client when opening the room with ludeo_Session_OpenRoom
	 */
	const char* roomId;

	/**
	 * If playing a ludeo, then this is set. Nulled otherwise
	 */
	const char* ludeoId;
));


//
// ludeo_Room_Close related types
//

/** The most recent version of ludeo_Room_Close */
#define LUDEO_ROOM_CLOSE_API_LATEST 1

/**
 * Structure with the parameters for ludeo_Room_Close
 */
LUDEO_STRUCT(LudeoRoomCloseParams, (
	/** Set this to LUDEO_ROOM_CLOSE_API_LATEST */
	int32_t apiVersion;
));

/** The parameters to the ludeo_Room_Close callback */
LUDEO_STRUCT(LudeoRoomCloseCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoRoomCloseCallback, const LudeoRoomCloseCallbackParams* data);


//
// ludeo_Room_AddPlayer related types
//

/** The most recent version of ludeo_Room_AddPlayer */
#define LUDEO_ROOM_ADDPLAYER_API_LATEST 1

/**
 * Structure with the parameters for ludeo_Room_AddPlayer
 */
LUDEO_STRUCT(LudeoRoomAddPlayerParams, (
	/** Set this to LUDEO_ROOM_ADDPLAYER_API_LATEST */
	int32_t apiVersion;

	/**
	 * Player Id to add
	 * The SDK doesn't care what the actual string is. It is up to the client.
	 */
	const char* playerId;

	// #LC3636  : Discussed some of this with Boaz, and we might need a ludeoUser field (or array if adding several players). (Private discussion in https://ludeos-workspace.slack.com/archives/D069AHNHPRV/p1705493623843459 )
));

/** The parameters to the ludeo_Room_AddPlayer callback */
LUDEO_STRUCT(LudeoRoomAddPlayerCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;

	// #LC3636 : I'm unsure if these should be here or somewhere else (e.g: when the room is ready). To be decided. Decision will
	// based on the expected lifetime of these. Maybe these should be given to the client as part of the "RoomReady" notification

	/**
	 * GameplaySession handles to use for the player
	 * If any error occurred, then this will be null.
	 *
	 * The lifetime of this handle is tied to the lifetime of the owning Room.
	 * Any handles not destroyed when the room is destroyed will be destroyed and invalidated automatically.
	 */
	LudeoHGameplaySession gameplaySession;
));

LUDEO_DECLARE_CALLBACK(LudeoRoomAddPlayerCallback, const LudeoRoomAddPlayerCallbackParams* data);

//
// ludeo_Room_RemovePlayer related types
//

/** The most recent version of ludeo_Room_RemovePlayer */
#define LUDEO_ROOM_REMOVEPLAYER_API_LATEST 1

/**
 * Structure with the parameters for ludeo_Room_RemovePlayer
 */
LUDEO_STRUCT(LudeoRoomRemovePlayerParams, (
	/** Set this to LUDEO_ROOM_REMOVEPLAYER_API_LATEST */
	int32_t apiVersion;

	/**
	 * Player to remove.
	 * This is what was passed to to the ludeo_Room_AddPlayer API
	 */
	const char* playerId;
));

LUDEO_STRUCT(LudeoRoomRemovePlayerCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoRoomRemovePlayerCallback, const LudeoRoomRemovePlayerCallbackParams* data);


#pragma pack(pop)

