// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/DataTypesCommon.h"

#pragma pack(push, 8)

/** Opaque DataWriter handle */
EXTERN_C typedef struct LudeoDataWriterHandle* LudeoHDataWriter;

//
// ludeo_DataWriter_SetSendSettings related types
//

/** The most recent version of the ludeo_DataWriter_SetSendSettings API */
#define LUDEO_DATAWRITER_SETSENDSETTINGS_API_LATEST 1

/** The parameters to ludeo_DataWriter_SetValues */
LUDEO_STRUCT(LudeoDataWriterSetSendSettingsParams, (
	/** Set this to LUDEO_DATAWRITER_SETSENDSETTINGS_API_LATEST */
	int32_t apiVersion;

	/** 
	 * Interval between sends, in milliseconds.
	 * In practice this won't be exact, because data sending only occurs as the result of a ludeo_Tick.
	 *
	 * For performance and bandwidth reasons, the SDK might impose a limit on how small this value can be.
	 */
	uint32_t sendIntervalMs;
));

//
// ludeo_DataWriter_CreateObject related types
//

/** The most recent version of the ludeo_DataWriter_CreateObject API */
#define LUDEO_DATAWRITER_CREATEOBJECT_API_LATEST 1

LUDEO_STRUCT(LudeoDataWriterCreateObjectParams, (
	/** Set this to LUDEO_DATAWRITER_CREATEOBJECT_API_LATEST */
	int32_t apiVersion;

	/**
	 * String that that identifies the object type.
	 * The SDK doesn't impose any restrictions on this. Meaning is up to the client.
	 */
	 const char* objectType;
));

//
// ludeo_DataWriter_DestroyObject related types
//

/** The most recent version of the ludeo_DataWriter_DestroyObject API */
#define LUDEO_DATAWRITER_DESTROYOBJECT_API_LATEST 1

LUDEO_STRUCT(LudeoDataWriterDestroyObjectParams, (
	/** Set this to LUDEO_DATAWRITER_DESTROYOBJECT_API_LATEST */
	int32_t apiVersion;

	/**
	 * Object to destroy
	 */
	LudeoObjectId objectId;
));

//
// ludeo_DataWriter_SendAction related types
//

/** The most recent version of the ludeo_DataWriter_DestroyObject API */
#define LUDEO_DATAWRITER_SENDACTION_API_LATEST 1

LUDEO_STRUCT(LudeoDataWriterSendActionParams, (
	/** Set this to LUDEO_DATAWRITER_SENDACTION_API_LATEST */
	int32_t apiVersion;

	/**
	 * Player id the action applies to
	 */
	const char* playerId;

	/**
	 * The action to send
	 */
	const char* action;
));

#pragma pack(pop)

