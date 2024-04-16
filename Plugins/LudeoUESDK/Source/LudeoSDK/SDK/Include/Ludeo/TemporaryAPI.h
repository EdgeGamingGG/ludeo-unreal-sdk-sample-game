#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/SessionTypes.h"

// #RVF : We need to delete this file (And all the SDK code) related to these functions
// This is just to temporary to expose functions to the UE plugin

// Returns the auth token used to authenticate with the backend
LUDEO_DECLARE_FUNC(const char*) ludeo_Session_TEMP_GetAuthToken(LudeoHSession handle);

// Returns what is called by the backend as "gameSessionId". It's the Id given to the session when we log in
// Returns the string or an empty string on error
LUDEO_DECLARE_FUNC(const char*) ludeo_Session_TEMP_GetSessionId(LudeoHSession handle);

// Returns what the backend calls `gameId`.
// Returns the string, or empty string on error.
LUDEO_DECLARE_FUNC(const char*) ludeo_Session_TEMP_GetGameId(LudeoHSession handle);

// Returns what is called by the backend as "gameplayId"
// Returns the string or an empty string on error
LUDEO_DECLARE_FUNC(const char*) ludeo_Session_TEMP_GetGameplayId(LudeoHSession handle);

// Returns the URL of the last highlight or an empty string on error
LUDEO_DECLARE_FUNC(const char*) ludeo_Session_TEMP_GetLastHighlightUrl(LudeoHSession handle);

// Returns the Id of the last highlight or an empty string on error
LUDEO_DECLARE_FUNC(const char*) ludeo_Session_TEMP_GetLastHighlightId(LudeoHSession handle);


//
// Temporary notification for highlight.
//

/** The most recent version of the ludeo_Session_AddNotifyResumeGameRequest API */
#define LUDEO_SESSION_ADDNOTIFYHIGHLIGHT_API_LATEST 1

/** The parameters to ludeo_Session_AddNotifyHighlight */
LUDEO_STRUCT(LudeoSessionAddNotifyHighlightParams, (
	/** Set this to LUDEO_SESSION_ADDNOTIFYHIGHLIGHT_API_LATEST */
	int32_t apiVersion;
));

/**
 * Structure with the parameters passed to the Highlight callback
 */
LUDEO_STRUCT(LudeoSessionHighlightCallbackParams, (
	void *clientData;
	const char* highlightUrl;
	const char* highlightId;
));

LUDEO_DECLARE_CALLBACK(LudeoSessionHighlightCallback, const LudeoSessionHighlightCallbackParams* data);

LUDEO_DECLARE_FUNC(LudeoNotificationId) ludeo_Session_AddNotifyHighlight(LudeoHSession handle, const LudeoSessionAddNotifyHighlightParams* params, void* clientData, LudeoSessionHighlightCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_ADDNOTIFY(Session, SESSION, AddNotifyHighlight, ADDNOTIFYHIGHLIGHT, Highlight);

