// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "InitTypes.h"

/**
 * Initializes the SDK.
 * 
 * Clients must call this function before calling any other SDK functions.
 * It should be called just once, and with a matching ludeo_Shutdown call
 *
 * @param params Structure containing the parameters
 * @return A LudeoResult to indicate success or failure
 * 
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_Initialize(const LudeoInitializeParams* params);
LUDEO_DEFINE_FUNC_TRAITS(Initialize, INITIALIZE);

/**
 * Shuts down the SDK
 *
 * After calling this, clients should not call any other SDK functions with the exception of ludeo_ResultToString.
 *
 * If there are any pending tasks (internal or externally triggered), the SDK will tick itself for a few seconds to attempt
 * gracefully finishing those tasks. If after that time window there are tasks pending, it will forcibly cancel them.
 * Therefore, any async calls the client started but haven't yet completed can have their callbacks called as a result of a 
 * ludeo_Shutdown call.
 *
 * The client should use the clientData context provided for those callbacks to determine if it needs to acts on those callbacks
 * or if it can just ignore them.
 *
 * @return A LudeoResult to indicate success or failure
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_Shutdown();

/**
 * Returns a string representation of a LudeoResult.
 *
 * The returned string is never null and should not be freed.
 */
LUDEO_DECLARE_FUNC(const char*) ludeo_ResultToString(LudeoResult result);

/**
 * Sets the logging callback function.
 * Any logging done by the SDK will be forwarded to the specified function.
 * Avoid expensive operations from the callback, such as directly logging to a file.
 *
 * IMPORTANT: This callback can be called from any of the SDK's threads.
 *
 * @param callback Callback to use for logging. This replaces any previously set callback. If nulled, it will remove any
 *		previously set callback.
 * @return A LudeoResult to indicate success or failure
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_SetLoggingCallback(LudeoLogCallback callback);

/**
 * Enables or disables logging to a file.
 *
 * The SDK can automatically log to a file to simplify development, but that is disabled by default.
 * This function allows enabling or disabling that feature.
 *
 * If a file named LudeoSDK.log already exists, the SDK will attempt to rename that existing file by adding the last write
 * UTC time of that file to its name. If for any reason the rename fails (e.g: OS related permissions), LudeoResult::Unknown is 
 * returned.
 *
 * @param params Structure containing the parameters
 * @return A LudeoResult to indicate success or failure.
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_SetLoggingToFile(const LudeoSetLoggingToFileParams* params);
LUDEO_DEFINE_FUNC_TRAITS(SetLoggingToFile, SETLOGGINGTOFILE);

/**
 * Sets the maximum log level for the a log category
 * By default all categories will log at the LudeoLogLevel::Log level or bellow, which means Verbose
 * or VeryVerbose levels will not show up in the logs.
 * 
 * Although the client can filter logs in its logging callback, it is recommended to use this function instead.
 * This is because an unneeded log message at the callback level means wasted performance formatting the message, while
 * a log message filtered with this function will cause the message formatting to be skipped altogether.
 * 
 * @param category Log category to change . The special value LudeoLogCategory::All can be specified to set all categories
 * @param level Maximum log level to report
 * @return
 *		A LudeoResult to indicate success or failure. The only reason the function can fail is if the category value passed is
 *		an invalid enum value.
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_SetLoggingLevel(LudeoLogCategory category, LudeoLogLevel level);

/**
 * Allows the SDK to do work.
 * This needs to be called periodically, ideally once per game tick.
 * 
 * Any callbacks are called only from this function. This allows the client to control when it can receive callbacks, providing
 * a well defined threading model.
 *
 * If the SDK has not been initialized, this does nothing.
 */
LUDEO_DECLARE_FUNC(void) ludeo_Tick();

/**
 * Removes a notification that the client added with any of the ludeo_<INTERFACE>_AddNotifyXXXXX methods;
 * If the notification id has a value of LUDEO_INVALID_NOTIFICATIONID, then the call is a no-op.
 */
LUDEO_DECLARE_FUNC(void) ludeo_RemoveNotification(LudeoNotificationId notificationId);

/**
 * 
 * This function should typically not be used for shipping.
 * It allows tweaking some of the SDK internals to help debug issues.
 * A client should NOT rely on any of these command to be available with newer versions of the SDK.
 * 
 * Available commands
 *
 *  "overlay-enabled"
 *		Description:
 *			Allows the client to keep the overlay from initializing. Disabling the overlay means you can't have an actual ludeo
 *			experience, but it can be useful to diagnose problems.
 *			This should be called before ludeo_Initialize. Once the overlay is enabled it can't be disabled.
 *		Value:
 *			0 - Overlay will be disabled
 *			1 (default) - Overlay will be enabled
 *		Example:
 *			ludeo_Command("overlay-enabled", "0");
 * 
 * "monitor-enabled"
 *		Description:
 *			The SDK monitors some performance related stats such as FPS and frametime variance and sends them to the backend at
 *			regular intervals.
 *			This command allows disabling monitor. This can be useful during development to disable any unnecessary outgoing
 *			http requests and to further reduce logging noise.
 *			It should be called before ludeo_Initialize.
 *		Value:
 *			0 - Disable monitoring.
 *			1 - (default) Doesn't do anything, since monitoring is only enabled if the backend tells the SDK to enable it.
 *
 *  "video-enabled"
 *		Description:
 *			Allows the client to disable recording. This can be used during debug
 *			This should be called before ludeo_Initialize.
 *		Value:
 *			0 - Video recording will be disabled
 *			1 (default) - Video recording will be enabled
 *		Example:
 *			ludeo_Command("video-enabled", "0");
 *
 * "video-localsave"
 *		Description:
 *			If set to true, it will cause videos to be saved locally as a raw h264 file before uploading. The videos will be saved
 *			as LudeoVideoMessage-<UTC TIMESTAMP>.h264 files in the current working directory.
 *			Note that this should ONLY be used for debugging.
 *			This can be called at any time, and the change will affect any videos created after the command.
 *		Value:
 *			0 - (default) Videos are not saved to files
 *			1 - Videos are saved to files before uploading
 *
 * "video-swencoder"
 *		Description:
 *			Typically, the SDK tries to use any supported hardware encoders, and the software encoder is the fallback encoder.
 *			Setting this to true forces the use of the software encoder.
 *			It should be called before ludeo_Initialize
 *		Value:
 *			0 - (default) Will use an hardware encoder if available
 *			1 - Will use the software encoder.
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_Command(const char* name, const char* value);

