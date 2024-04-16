// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"

#pragma pack(push, 8)

/** Opaque GameplaySession handle */
EXTERN_C typedef struct LudeoGameplaySessionHandle* LudeoHGameplaySession;

//
// ludeo_GameplaySession_GetInfo related types
//

/** The most recent version of ludeo_Room_GetInfo */
#define LUDEO_GAMEPLAYSESSION_GETINFO_API_LATEST 1

LUDEO_STRUCT(LudeoGameplaySessionGetInfoParams, (
	/** Set this to LUDEO_GAMEPLAYSESSION_GETINFO_API_LATEST */
	int32_t apiVersion;
));


/** Latest version of the LudeoGameplaySessionInfo struct */
#define LUDEO_GAMEPLAYSESSIONINFO_API_LATEST 1

/** 
 * Information that can be retrieved with the ludeo_GameplaySession_GetInfo function
 *
 * The client should call ludeo_GameplaySessionInfo_Release to release the structure.
 */
LUDEO_STRUCT(LudeoGameplaySessionInfo,(

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
	 * Player id supplied to the ludeo_Room_AddPlayer
	 */
	const char* playerId;

	// #ROOMS : Add a "state" or similar field to indicate what's the state of things (Ready, Gameplay), etc.
));


//
// ludeo_GameplaySession_Begin related types
//

/** The most recent version of ludeo_GameplaySession_Begin */
#define LUDEO_GAMEPLAYSESSION_BEGIN_API_LATEST 1

LUDEO_STRUCT(LudeoGameplaySessionBeginParams, (
	/** Set this to LUDEO_GAMEPLAYSESSION_BEGIN_API_LATEST */
	int32_t apiVersion;
));

/** The parameters to the ludeo_GameplaySession_Begin callback */
LUDEO_STRUCT(LudeoGameplaySessionBeginCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoGameplaySessionBeginCallback, const LudeoGameplaySessionBeginCallbackParams* data);


//
// ludeo_GameplaySession_End related types
//

/** The most recent version of ludeo_GameplaySession_End */
#define LUDEO_GAMEPLAYSESSION_END_API_LATEST 1

LUDEO_STRUCT(LudeoGameplaySessionEndParams, (
	/** Set this to LUDEO_GAMEPLAYSESSION_END_API_LATEST */
	int32_t apiVersion;

	/**
	 * This is only relevant when in a ludeo run.
	 * If the client wishes to programmatically end a ludeo run, the SDK needs to know what's the intent:
	 *
	 * - false (Not an abort) : The Ludeo summary overlay will be shown, so the ludeo can be replayed. A scenario for this is in a
	 *   last man standing game. If the player kills all other players or enemies, the client can end the ludeo without waiting
	 *   for the timer to end.
	 *
	 * - true - (abort) : The client wishes to exit the ludeo and NOT display the Ludeo Summary overlay.
	 */
	LudeoBool isAbort;
));

/** The parameters to the ludeo_GameplaySession_End callback */
LUDEO_STRUCT(LudeoGameplaySessionEndCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoGameplaySessionEndCallback, const LudeoGameplaySessionEndCallbackParams* data);

#pragma pack(pop)

