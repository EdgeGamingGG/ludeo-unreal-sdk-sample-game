// Copyright 2023 Ludeo. All rights reserved.

#pragma  once

#include "Ludeo/Common.h"
#include "Ludeo/RoomTypes.h"
#include "Ludeo/DataWriterTypes.h"
#include "Ludeo/DataReaderTypes.h"

#pragma pack(push, 8)

/** Opaque Session handle */
EXTERN_C typedef struct LudeoSessionHandle* LudeoHSession;

//
// ludeo_Session_Create related types
//

/** The most recent version of the ludeo_Session_Create API */
#define LUDEO_SESSION_CREATE_API_LATEST 1

/**
 * Structure with the parameters for ludeo_Session_Create
 */
LUDEO_STRUCT(LudeoSessionCreateParams, (

	/** Set this to LUDEO_SESSION_CREATE_API_LATEST */
	int32_t apiVersion;
));

//
// ludeo_Session_Release related types
//

/** The most recent version of the ludeo_Session_Release API */
#define LUDEO_SESSION_RELEASE_API_LATEST 1

/**
 * Structure with the parameters for ludeo_Session_Release
 */
LUDEO_STRUCT(LudeoSessionReleaseParams, (
	/** Set this to LUDEO_SESSION_RELEASE_API_LATEST */
	int32_t apiVersion;
));

/** The parameters to the ludeo_Session_Release callback */
LUDEO_STRUCT(LudeoSessionReleaseCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionReleaseCallback, const LudeoSessionReleaseCallbackParams* data);


//
// ludeo_Session_Activate related types
//

/**
 * User authentication type to use
 */
LUDEO_ENUM_START(LudeoAuthType)
	/**
	 * Explicit authentication done with Steam user details
	 */
	LUDEO_ENUM_VALUE(LudeoAuthType, Steam, 0)

	/**
	 * Explicit authentication done with Epic Games user details
	 *
	 * NOTE: Not yet implemented
	 */
	LUDEO_ENUM_VALUE(LudeoAuthType, Epic, 1)

	/**
	 * Authentication with GeForce NOW
	 * 
	 * This is used only internally. At the moment there is nothing for the client to specify explicitly. The SDK detects if
	 * running in the GeForceNow environment.
	 */
	LUDEO_ENUM_VALUE_LAST(LudeoAuthType, Nvidia, 2)

LUDEO_ENUM_END(LudeoAuthType);

/**
 * Structure with authentication details for ludeo_Session_Create when explicit authentication with Steam is wanted.
 */
LUDEO_STRUCT(LudeoSteamAuthDetails, (
	/** Needs to be set to LudeoAuthType::Steam */
	LudeoAuthType authType;

	/**
	 * Steam user ID
	 * You can retrieve this with Steamworks's SteamUser()->GetSteamID()
	 */
	const char* authId;

	 /**
	  * Steam beta branch
	  * You can retrieve this with Steamworks's SteamApps()->GetCurrentBetaName()
	  *
	  * This is optional. If nulled or an empty string, then it's not a beta branch
	  */
	  const char* currentBetaName;
));


// #ROOMS : Remove this
/**
 * The different modes a Session instance can be in
 */
 LUDEO_ENUM_START(LudeoSessionMode)

	// #LC3630 These values will probably have to change to be None, Idle, Running or something like that.

	/** The Session is currently just waiting */
	LUDEO_ENUM_VALUE(LudeoSessionMode, None, 0)

	/** The Session is in Creator mode */
	LUDEO_ENUM_VALUE(LudeoSessionMode, Creator, 1)

	/** The Session is in Player mode */
	LUDEO_ENUM_VALUE_LAST(LudeoSessionMode, Player, 2)

LUDEO_ENUM_END(LudeoSessionMode);

/** The most recent version of ludeo_Session_Activate API */
#define LUDEO_SESSION_ACTIVATE_API_LATEST 1

/**
 * Structure with the parameters for ludeo_Session_Activate
 */
LUDEO_STRUCT(LudeoSessionActivateParams, (

	/** Set this to LUDEO_SESSION_ACTIVATE_API_LATEST */
	int32_t apiVersion;

	/**
	 * Ludeo platform's url to use. This can be for example https://staging-services.ludeo.com or https://services.ludeo.com
	 */
	const char* platformUrl;

	/**
	 * Pointer to one of the Ludeo<NAME>AuthDetails structs or nulled.
	 *
	 * If nulled, the SDK will try to automatically deduce authentication based on the current platform/environment.
	 * For example, on Platforms that support Steam, the SDK will check if the Steamworks dll is loaded and attempts to retrieve
	 * the user information. 
	 * Note that the SDK doesn't initialize Steamworks (or any other supported provider). That's the responsibility of the client.
	 * If the client didn't initialize any of the supported providers (e.g: Steamworks), then authentication will fail with
	 * LudeoResult::InvalidAuth.
	 * 
	 * Explicit authentication details allows working around any unforeseen problems with either the SDK or client design, if
	 * for some reason the SDK can't retrieve the required information.
	 * Also useful during development or for testing scenarios where e.g Steam might not be running.
	 */
	void* authDetails;

	/** Api key to access Ludeo services
	 * This is dependent on the url specified in platformUrl
	 */
	const char* apiKey;

	/**
	 * Backend app token to activate if available.
	 * It can be nulled.
	 * 
	 * Normally, this will cause the backend to signal the SDK that it should play a Ludeo, which will cause the ludeoSelected 
	 * field in the callback parameters struct will be set to true, and a LudeoSelected notification will occur.
	 */
	const char* appToken;

	// #ROOMS : This should probably NOT be at the session level. Probably should be in the SDK initialization

	/**
	 * The client's window handle (Native OS handle) that should be used for the SDK's overlays and video recording.
	 */
	void* windowHandle;

	/**
	 * If true, it will reset the attributes and actions the backend has for this game.
	 *
	 * This functionality is a no-op in the SDK's Release build to keep the client from unintentionally resetting a live game.
	 */
	LudeoBool reset;
));

LUDEO_STRUCT(LudeoSessionActivateCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;

	/**
	 * If as part of the session activation the SDK detected that a ludeo should be played, this is set to true.
	 * When this is true, the SDK guarantees that soon after this callback, a LudeoSelected notification is received.
	 */
	LudeoBool ludeoSelected;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionActivateCallback, const LudeoSessionActivateCallbackParams* data);

//
// ludeo_Session_AddNotifyLudeoSelected related types
//

/** The most recent version of the ludeo_Session_AddNotifyLudeoSelected API */
#define LUDEO_SESSION_ADDNOTIFYLUDEOSELECTED_API_LATEST 1

/** The parameters to ludeo_Session_AddNotifyLudeoSelected */
LUDEO_STRUCT(LudeoSessionAddNotifyLudeoSelectedParams, (
	/** Set this to LUDEO_SESSION_ADDNOTIFYLUDEOSELECTED_API_LATEST */
	int32_t apiVersion;
));

/**
 * Structure with the parameters passed to the callback for when a ludeo is selected.
 * See ludeo_Session_AddNotifyLudeoSelected for more details.
 */
LUDEO_STRUCT(LudeoSessionLudeoSelectedCallbackParams, (

	/** Context that was passed to ludeo_Session_AddNotifyLudeoSelected */
	void *clientData;

	/**
	 * Id of the selected ludeo.
	 * If the client wishes to play the ludeo, it should pass this to ludeo_Session_GetLudeo to get the actual ludeo data.
	 */
	const char* ludeoId;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionLudeoSelectedCallback, const LudeoSessionLudeoSelectedCallbackParams* data);


//
// ludeo_Session_AddNotifyPauseGameRequest related types
//

/** The most recent version of the ludeo_Session_AddNotifyPauseGameRequest API */
#define LUDEO_SESSION_ADDNOTIFYPAUSEGAMEREQUEST_API_LATEST 1

/** The parameters to ludeo_Session_AddNotifyPauseGameRequest */
LUDEO_STRUCT(LudeoSessionAddNotifyPauseGameRequestParams, (
	/** Set this to LUDEO_SESSION_ADDNOTIFYPAUSEGAMEREQUEST_API_LATEST */
	int32_t apiVersion;
));

/**
 * Structure with the parameters passed to the PauseGameRequest callback
 * See ludeo_Session_AddNotifyPauseGameRequest for more details.
 */
LUDEO_STRUCT(LudeoSessionPauseGameRequestCallbackParams, (

	/** Context that was passed to ludeo_Session_AddNotifyPauseGameRequest */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionPauseGameRequestCallback, const LudeoSessionPauseGameRequestCallbackParams* data);

//
// ludeo_Session_AddNotifyResumeGameRequest related types
//

/** The most recent version of the ludeo_Session_AddNotifyResumeGameRequest API */
#define LUDEO_SESSION_ADDNOTIFYRESUMEGAMEREQUEST_API_LATEST 1

/** The parameters to ludeo_Session_AddNotifyResumeGameRequest */
LUDEO_STRUCT(LudeoSessionAddNotifyResumeGameRequestParams, (
	/** Set this to LUDEO_SESSION_ADDNOTIFYRESUMEGAMEREQUEST_API_LATEST */
	int32_t apiVersion;
));

/**
 * Structure with the parameters passed to the ResumeGameRequest callback
 * See ludeo_Session_AddNotifyResumeGameRequest for more details.
 */
LUDEO_STRUCT(LudeoSessionResumeGameRequestCallbackParams, (

	/** Context that was passed to ludeo_Session_AddNotifyResumeGameRequest */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionResumeGameRequestCallback, const LudeoSessionResumeGameRequestCallbackParams* data);


//
// ludeo_Session_AddNotifyBackToMenuRequest related types
//

/** The most recent version of the ludeo_Session_AddNotifyBackToMenuRequest API */
#define LUDEO_SESSION_ADDNOTIFYBACKTOMENUREQUEST_API_LATEST 1

/** The parameters to ludeo_Session_AddNotifyBackToMenuRequest */
LUDEO_STRUCT(LudeoSessionAddNotifyBackToMenuRequestParams, (
	/** Set this to LUDEO_SESSION_ADDNOTIFYBACKTOMENUREQUEST_API_LATEST */
	int32_t apiVersion;
));

/**
 * Structure with the parameters passed to the BackToMenuRequest callback
 * See ludeo_Session_AddNotifyBackToMenuRequest for more details.
 */
LUDEO_STRUCT(LudeoSessionBackToMenuRequestCallbackParams, (

	/** Context that was passed to ludeo_Session_AddNotifyBackToMenuRequest */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionBackToMenuRequestCallback, const LudeoSessionBackToMenuRequestCallbackParams* data);

//
// ludeo_Session_AddNotifyRoomReady related types
//

/** The most recent version of the ludeo_Session_AddNotifyRoomReady API */
#define LUDEO_SESSION_ADDNOTIFYROOMREADY_API_LATEST 1

/** The parameters to ludeo_Session_AddNotifyRoomReady */
LUDEO_STRUCT(LudeoSessionAddNotifyRoomReadyParams, (
	/** Set this to LUDEO_SESSION_ADDNOTIFYROOMREADY_API_LATEST */
	int32_t apiVersion;
));

/**
 * Structure with the parameters passed to the RoomReady callback
 * See ludeo_Session_AddNotifyRoomReady for more details.
 */
LUDEO_STRUCT(LudeoSessionRoomReadyCallbackParams, (

	/** Context that was passed to ludeo_Session_AddNotifyRoomReady */
	void *clientData;

	/**
	 * Handle of the room this notification refers to.
	 * Note that this is NOT a new handle. It's the handle to an existing room the client created.
	 *
	 * Even if the client only has 1 room at a time, it should still validate if the notification is for the room it expects.
	 * This can avoid bugs related to a callback being in the queue for execution but when the client handles the callback that
	 * room is no longer relevant.
	 * If the client needs to query other info about the room, it can use the ludeo_Room_GetInfo function.
	 */
	LudeoHRoom room;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionRoomReadyCallback, const LudeoSessionRoomReadyCallbackParams* data);

/** The most recent version of the ludeo_Session_AddNotifyRoomReady API */
#define LUDEO_SESSION_ADDNOTIFYCONSENTUPDATED_API_LATEST 1

/** The parameters to ludeo_Session_AddNotifyConsentUpdated */
LUDEO_STRUCT(LudeoSessionAddNotifyConsentUpdatedParams, (
	/** Set this to LUDEO_SESSION_ADDNOTIFYCONSENTUPDATED_API_LATEST */
	int32_t apiVersion;
));

/**
 * Structure with the parameters passed to the ConsentUpdated callback
 * See ludeo_Session_AddNotifyConsentUpdated for more details.
 */
LUDEO_STRUCT(LudeoSessionConsentUpdatedCallbackParams, (

	/** Context that was passed to ludeo_Session_AddNotifyConsentUpdated */
	void *clientData;

	/** Flag indicates if remote screens are enabled */
	LudeoBool remoteScreens;

	/** Flag indicates if monitoring data are reported */
	LudeoBool reportMonitor;

	/** Flag indicates if screen is captured */
	LudeoBool captureScreen;

	/** Flag indicates if gameplay data can be streamed to room */
	LudeoBool captureData;

	/** Flag indicates if highlight can be captured */
	LudeoBool canCaptureHighlight;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionConsentUpdatedCallback, const LudeoSessionConsentUpdatedCallbackParams* data);

//
// ludeo_Session_GetLudeo related types
//

/** The most recent version of the ludeo_Session_GetLudeo API */
#define LUDEO_SESSION_GETLUDEO_API_LATEST 1

/** The parameters to ludeo_Session_GetLudeo */
LUDEO_STRUCT(LudeoSessionGetLudeoParams, (
	/** Set this to LUDEO_SESSION_GETLUDEO_API_LATEST */
	int32_t apiVersion;

	/** String with the ludeo Id to retrieve */
	const char* ludeoId;
));

/** The parameters to the ludeo_Session_GetLudeo callback */
LUDEO_STRUCT(LudeoSessionGetLudeoCallbackParams, (
	/**
	 * Tells if the operation succeeded or failed.
	 * If it succeeded, then dataReader contains the handle of the DataReader interface to use.
	 */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;

	/**
	 * If resultCode is Ludeo::Success, this contains the handle the client can use to read back the game state.
	 * The client can keep this handle for the entire lifetime of the owning Session instance, even between different 
	 * creator and player runs.
	 *
	 * The suggested approach is to keep the handle only while doing repeated ludeo player runs, and once switching to different ludeo
	 * or creator mode call ludeo_DataReader_Release to let the SDK know that ludeo data is no longer needed.
	 * The SDK is then free to release that memory in order to decrease memory usage.
	 */
	LudeoHDataReader dataReader;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionGetLudeoCallback, const LudeoSessionGetLudeoCallbackParams* data);

/** The most recent version of the ludeo_Session_OpenRoom API */
#define LUDEO_SESSION_OPENROOM_API_LATEST 1

/** Structure with the parameters to ludeo_Session_OpenRoom */
LUDEO_STRUCT(LudeoSessionOpenRoomParams, (
	/** Set this to LUDEO_SESSION_OPENROOM_API_LATEST */
	int32_t apiVersion;

	/**
	 * A string that uniquely identifies the room.
	 * If specified, then this needs to be unique, such as a UUID.
	 * If not specified, then the SDK will generate a UUID. The client can then query the API to retrieve it.
	 */
	const char* roomId;

	/**
	 * If specified, it should be a valid Ludeo UUID
	 * If nulled, the room is opened to play the game normally (not a ludeo)
	 */
	const char* ludeoId;
));

/** The parameters to the ludeo_Session_OpenRoom callback */
LUDEO_STRUCT(LudeoSessionOpenRoomCallbackParams, (

	/**
	 * Tells if the operation succeeded or failed.
	 * If this is LudeoResult::SDKDisabled, the player hasn't consented yet to the required SDK features, such as data capturing.
	 */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;

	/** 
	 * Room handle.
	 * This is used to call the ludeo_Room_XXX API.
	 */
	LudeoHRoom room;

	/**
	 * Handle to the DataWriter instance to use for the room
	 * This handle is only valid during this specific creator run.
	 */
	LudeoHDataWriter dataWriter;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionOpenRoomCallback, const LudeoSessionOpenRoomCallbackParams* data);

/** The most recent version of the ludeo_Session_BeginCreator API */
#define LUDEO_SESSION_BEGINCREATOR_API_LATEST 1

/** Structure with the parameters to ludeo_Session_BeginCreator */
LUDEO_STRUCT(LudeoSessionBeginCreatorParams, (
	/** Set this to LUDEO_SESSION_BEGINCREATOR_API_LATEST */
	int32_t apiVersion;
));

/** The parameters to the ludeo_Session_BeginCreator callback */
LUDEO_STRUCT(LudeoSessionBeginCreatorCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;

	// #RVF : Add better documentation here, explaining the lifetime of the handle
	/**
	 * Handle to the DataWriter instance to use during the creator run.
	 * This handle is only valid during this specific creator run.
	 */
	LudeoHDataWriter dataWriter;
));


LUDEO_DECLARE_CALLBACK(LudeoSessionBeginCreatorCallback, const LudeoSessionBeginCreatorCallbackParams* data);

/** The most recent version of the ludeo_Session_BeginLudeo API */
#define LUDEO_SESSION_BEGINLUDEO_API_LATEST 1

/** Structure with the parameters to ludeo_Session_BeginLudeo */
LUDEO_STRUCT(LudeoSessionBeginLudeoParams, (
	/** Set this to LUDEO_SESSION_BEGINLUDEO_API_LATEST */
	int32_t apiVersion;

	/** Ludeo Id to begin playing */
	const char* ludeoId;
));

/** Parameters to the ludeo_Session_BeginLudeo callback */
LUDEO_STRUCT(LudeoSessionBeginLudeoCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;

	/**
	 * Handle to the DataWriter instance to use during this ludeo run.
	 * This handle is only valid during this run.
	 */
	LudeoHDataWriter dataWriter;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionBeginLudeoCallback, const LudeoSessionBeginLudeoCallbackParams* data);

/** The most recent version of the ludeo_Session_BeginGameplay API */
#define LUDEO_SESSION_BEGINGAMEPLAY_API_LATEST 1

/** Structure with the parameters to ludeo_Session_BeginGameplay */
LUDEO_STRUCT(LudeoSessionBeginGameplayParams, (
	/** Set this to LUDEO_SESSION_BEGINGAMEPLAY_API_LATEST */
	int32_t apiVersion;
));

/** Parameters to the ludeo_Session_BeginGameplay callback */
LUDEO_STRUCT(LudeoSessionBeginGameplayCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionBeginGameplayCallback, const LudeoSessionBeginGameplayCallbackParams* data);

/** The most recent version of the ludeo_Session_Markhighlight API */
#define LUDEO_SESSION_MARKHIGHLIGHT_API_LATEST 1

/** Structure with the parameters to ludeo_Session_MarkHighlight */
LUDEO_STRUCT(LudeoSessionMarkHighlightParams, (
	/** Set this to LUDEO_SESSION_MARKHIGHLIGHT_API_LATEST */
	int32_t apiVersion;
));

/** Parameters to the ludeo_Session_MarkHighlight callback */
LUDEO_STRUCT(LudeoSessionMarkHighlightCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionMarkHighlightCallback, const LudeoSessionMarkHighlightCallbackParams* data);

/** The most recent version of the ludeo_Session_EndRun API */
#define LUDEO_SESSION_ENDRUN_API_LATEST 1

/** Structure with the parameters to ludeo_Session_EndRun */
LUDEO_STRUCT(LudeoSessionEndRunParams, (
	/** Set this to LUDEO_SESSION_ENDRUN_API_LATEST */
	int32_t apiVersion;
));

/** Parameters to the ludeo_Session_EndRun callback */
LUDEO_STRUCT(LudeoSessionEndRunCallbackParams, (
	/** Tells if the operation succeeded or failed. */
	LudeoResult resultCode;

	/** Context that was passed to the originating SDK call */
	void *clientData;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionEndRunCallback, const LudeoSessionEndRunCallbackParams* data);

/** The most recent version of the ludeo_Session_OpenGallery API */
#define LUDEO_SESSION_OPENGALLERY_API_LATEST 1

/** Structure with the parameters to ludeo_Session_OpenGallery */
LUDEO_STRUCT(LudeoSessionOpenGalleryParams, (
	/** Set this to LUDEO_SESSION_OPENGALLERY_API_LATEST */
	int32_t apiVersion;
));


#pragma pack(pop)

