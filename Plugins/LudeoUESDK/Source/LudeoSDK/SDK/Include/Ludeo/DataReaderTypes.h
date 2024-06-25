// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/DataTypesCommon.h"

#pragma pack(push, 8)

/** Opaque DataWriter handle */
EXTERN_C typedef struct LudeoDataReaderHandle* LudeoHDataReader;

//
// ludeo_DataReader_GetInfo related types
//

/** The most recent version of the ludeo_DataReader_GetInfo API */
#define LUDEO_DATAREADER_GETINFO_API_LATEST 1

/** The parameters to ludeo_DataReader_GetInfo */
LUDEO_STRUCT(LudeoDataReaderGetInfoParams, (
	/** Set this to LUDEO_DATAREADER_GETINFO_API_LATEST */
	int32_t apiVersion;
));


/** Latest version of the LudeoDataReaderInfo struct */
#define LUDEO_DATAREADERINFO_API_LATEST 1

/**
 * Structure used as an out parameter for the ludeo_DataReader_GetInfo.
 */
LUDEO_STRUCT(LudeoDataReaderInfo, (
	/**
	 * Set by the SDK when allocating the struct
	 */
	int32_t apiVersion;

	/**
	 * The id of the ludeo the DataReader refers to.
	 * The client should NOT keep hold of this pointer. It should copy the string instead of keeping the pointer.
	 */
	const char* ludeoId;

	/**
	 * Id of the player that created the ludeo.
	 */
	const char* playerId;
));

//
// ludeo_DataReader_GetObjectsInfo related types
//

/** Latest version of the LudeoObjectInfo struct */
#define LUDEO_OBJECTINFO_API_LATEST 1

/** Information about a single object */
LUDEO_STRUCT(LudeoObjectInfo, (
	/**
	 * Set by the SDK when allocating the struct
	 */
	int32_t apiVersion;

	/**
	 * Object Id.
	 */
	LudeoObjectId objectId;

	/**
	 * String that identifies the object type.
	 * This is what the client used when creating this object during the gameplay that created this ludeo data.
	 */
	const char* objectType;
));

/** Latest version of the LudeoObjectsInfo struct */
#define LUDEO_OBJECTSINFO_API_LATEST 1

/** Information about a group of objects */
LUDEO_STRUCT(LudeoObjectsInfo, (

	/**
	 * Set by the SDK when allocating the struct
	 */
	int32_t apiVersion;

	/** Array with the information about all the objects */
	LudeoObjectInfo* objects;

	/** How many objects in the array */
	uint32_t objectsCount;
));

/** The most recent version of the ludeo_DataReader_GetObjectsInfo API */
#define LUDEO_DATAREADER_GETOBJECTSINFO_API_LATEST 1

/** Parameters to the function */
LUDEO_STRUCT(LudeoDataReaderGetObjectsInfoParams, (
	int32_t apiVersion;
));

/** The most recent version of the ludeo_DataReader_VisitAttributes API */
#define LUDEO_DATAREADER_VISITATTRIBUTES_API_LATEST 1

LUDEO_STRUCT(LudeoDataReaderVisitAttributesParams, (
	/** Set this to LUDEO_DATAREADER_VISITATTRIBUTES_API_LATEST */
	int32_t apiVersion;
));

/** The parameters to the callback specified in ludeo_DataReader_VisitAttributes*/
LUDEO_STRUCT(LudeoDataReaderVisitAttributesCallbackParams, (
	/** Context that was passed to the originating SDK call */
	void* clientData;
	
	/** Attribute name */
	const char* name;

	/**
	 * Type of attribute.
	 * If type is LudeoDataType::Component, the client can do a call to `ludeo_DataReader_EnterComponent`, visit the attributes inside that component, and afterwards call `ludeo_DataReader_LeaveComponent`. This allows the client to recursively visit all attributes.
	 */
	LudeoDataType type;
));

LUDEO_DECLARE_CALLBACK_RETVALUE(LudeoBool, LudeoDataReaderVisitAttributesCallback, const LudeoDataReaderVisitAttributesCallbackParams* data);

// #ROOMS : Revise/removed this after the Room refactor
#if 0

/** The most recent version of the ludeo_DataReader_Visit API */
#define LUDEO_DATAREADER_VISIT_API_LATEST 1

LUDEO_STRUCT(LudeoDataReaderVisitParams, (
	/** Set this to LUDEO_DATAREADER_VISIT_API_LATEST */
	int32_t apiVersion;
));

/**
 * Visit callback used for the ludeo_DataReader_Visit API
 */
LUDEO_DECLARE_CALLBACK(LudeoDataReaderVisitCallback, const LudeoDataEntry* data);

/** The most recent version of the ludeo_DataReader_GetValues API */
#define LUDEO_DATAREADER_GETVALUE_API_LATEST 1

/** The parameters to ludeo_DataReader_Get */
LUDEO_STRUCT(LudeoDataReaderGetValueParams, (
	/** Set this to LUDEO_DATAREADER_GETVALUE_API_LATEST */
	int32_t apiVersion;

	/**
	 * The name of the value to retrieve. If not found, the function will fail with LudeoResult::NotFound
	 */
	const char* key;

	/**
	 * Type of the data to read.
	 * If it doesn't match the type of what the DataReader instance is holding, the function call will fail with
	 * LudeoResult::NotFound
	 */
	LudeoDataType dataType;
));
#endif

#pragma pack(pop)

