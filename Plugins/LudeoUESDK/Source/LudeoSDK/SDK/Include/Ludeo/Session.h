// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/SessionTypes.h"

/**
 * Creates Ludeo session instance
 *
 * A Session instance represents a logged in user and their connection to the backend.
 * After calling this function the client should:
 *
 * 1. Register to receive any notifications with the ludeo_Session_AddNotifyXXX functions
 * 2. Call ludeo_Session_Activate to connect to the backend
 *
 * @param params Structure with the function parameters
 * @param outHandle Will contain the session handle on exit.
 * @return LudeoResult::Success if the session was created, in which case the output parameter handle will contain the handle
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_Session_Create(const LudeoSessionCreateParams* params, LudeoHSession* outHandle);
LUDEO_DEFINE_FUNC_TRAITS_2(Session, SESSION, Create, CREATE);


/**
 * Returns true or false depending if the handle is a valid handle.
 * This is useful for code that needs to quickly validate handles in a away that doesn't produce side effects. E.g: Debug code.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_Session_IsValid(LudeoHSession handle);


/**
 * Releases a Ludeo session instance
 *
 * The client should call this for every Session before calling ludeo_Shutdown.
 * After calling this function, the session handle should not be used again. Doing so is undefined behavior.
 *
 * Session destruction is done asynchronously, and the completion callback is guaranteed to be the last callback for the Session.
 *
 * Ongoing async calls for the Session will be Canceled and their callbacks called with LudeoResult::Canceled.
 * Some of those ongoing async calls might have already finished and are waiting for the client to tick the SDK, therefore
 * those will not receive LudeoResult::Canceled in the callback.
 * The client should implement whatever strategies it needs to avoid triggering more calls to this Session handle when he receives
 * those callbacks.
 *
 * Consult the provided samples for possible patterns.
 * 
 * @param handle Session to release
 * @param params Structure with any parameters the function needs
 * @param clientData Context that will be passed to callback
 * @param callback Callback to call once destruction is complete
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_Release(LudeoHSession handle, const LudeoSessionReleaseParams* params, void* clientData, LudeoSessionReleaseCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, Release, RELEASE);

/**
 * Activates a session.
 * 
 * The client should call this after creating the session and registering to all the notifications.
 * While not technically required to register to notifications before calling this, some notifications can happen as the
 * result of calling this function and therefore might be missed by the client.
 * 
 * @param handle Session  to activate
 * @param params Structure with any parameters the function needs
 * @param clientData Context that will be passed to a callback
 * @param callback Callback to call once the operation completes
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_Activate(LudeoHSession handle, const LudeoSessionActivateParams* params, void* clientData, LudeoSessionActivateCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, Activate, ACTIVATE);

/**
 * Asynchronously retrieves from the backend (or from the local cache if available) all the information needed to play a
 * specified Ludeo.
 *
 * A typical workflow for the client would be:
 * - Use ludeo_Session_AddNotifyLudeoSelected to register for ludeo selection notifications.
 *		- The user triggers a ludeo selection by some mechanism external to the game, which causes the LudeoSelected notification
 *		  to be called.
 * - Once the LudeoSelected notification callback is called, pass the supplied ludeoId to ludeo_Session_GetLudeo to retrieve that
 *   ludeo's data.
 * - The completion callback for ludeo_Session_GetLudeo provides the DataReader instance that can be used to setup the game state
 *   to play the ludeo.
 * 
 * Note that calling ludeo_Session_GetLudeo doesn't change the current state of the session.
 * The client is in control on when switch to ludeo player mode.
 *
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback 
 * @param callback Callback to call once the ludeo information is available. 
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_GetLudeo(LudeoHSession handle, const LudeoSessionGetLudeoParams* params, void *clientData, LudeoSessionGetLudeoCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, GetLudeo, GETLUDEO);


/**
 * Opens a new room
 *
 * On completion, an handle to the new room is provided, which can be used with the ludeo_Room_XXX API.
 * To close a room a call to ludeo_Room_Close should be made.
 * 
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback 
 * @param callback Callback to call once the operation completes.
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_OpenRoom(LudeoHSession handle, const LudeoSessionOpenRoomParams* param, void* clientData, LudeoSessionOpenRoomCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, OpenRoom, OPENROOM);

// #ROOMS : Remove BeginCreator once refactoring is done

/**
 * Signals the SDK to start the session as a creator
 * Once the operation completes successfully, the client can then start feeding data to the SDK trough the supplied DataReader
 * instance.
 *
 * To stop the creator mode, a call to ludeo_Session_EndRun should be made.
 *
 * The client should avoid making calls to ludeo_Session_BeginCreator, ludeo_Session_BeginLudeo or ludeo_Session_EndRun while
 * any of those is still in progress.
 * Doing so will fail with a LudeoResult::WrongState because the SDK knows there is one in progresss, but because that call will
 * fail straight away, the callback will be called BEFORE the callback of the first call (which is still in progress). This can be
 * confusing for the client to handle.
 *
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback 
 * @param callback Completion callback. If the resultCode is LudeoResult::Success, then the client can start feeding data to the
 *        SDK.
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_BeginCreator(LudeoHSession handle, const LudeoSessionBeginCreatorParams* params, void *clientData, LudeoSessionBeginCreatorCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, BeginCreator, BEGINCREATOR);

/**
 * Registers to receive a notification when a Ludeo is selected externally to the client.
 * E.g: This allows the client to know when the player triggers a ludeo from outside the game.
 *
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback 
 * @param callback Callback to call when a ludeo is selected
 *
 * @return
 * The notification Id. The client should call ludeo_RemoveNotification whenever it wants to stop receiving the notification.
 * The notification's lifetime is tied to the Session, the SDK will stop sending the notification once the Session is destroyed.
 * If any of the parameters are invalid and the call fails, it returns LUDEO_INVALID_NOTIFICATIONID
 *
 */
LUDEO_DECLARE_FUNC(LudeoNotificationId) ludeo_Session_AddNotifyLudeoSelected(LudeoHSession handle, const LudeoSessionAddNotifyLudeoSelectedParams* params, void* clientData, LudeoSessionLudeoSelectedCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_ADDNOTIFY(Session, SESSION, AddNotifyLudeoSelected, ADDNOTIFYLUDEOSELECTED, LudeoSelected);

// #RVF : Document this
LUDEO_DECLARE_FUNC(LudeoNotificationId) ludeo_Session_AddNotifyPauseGameRequest(LudeoHSession handle, const LudeoSessionAddNotifyPauseGameRequestParams* params, void* clientData, LudeoSessionPauseGameRequestCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_ADDNOTIFY(Session, SESSION, AddNotifyPauseGameRequest, ADDNOTIFYPAUSEGAMEREQUEST, PauseGameRequest);

// #RVF : Document this
LUDEO_DECLARE_FUNC(LudeoNotificationId) ludeo_Session_AddNotifyResumeGameRequest(LudeoHSession handle, const LudeoSessionAddNotifyResumeGameRequestParams* params, void* clientData, LudeoSessionResumeGameRequestCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_ADDNOTIFY(Session, SESSION, AddNotifyResumeGameRequest, ADDNOTIFYRESUMEGAMEREQUEST, ResumeGameRequest);

// #RVF : Document this
LUDEO_DECLARE_FUNC(LudeoNotificationId) ludeo_Session_AddNotifyBackToMenuRequest(LudeoHSession handle, const LudeoSessionAddNotifyBackToMenuRequestParams* params, void* clientData, LudeoSessionBackToMenuRequestCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_ADDNOTIFY(Session, SESSION, AddNotifyBackToMenuRequest, ADDNOTIFYBACKTOMENUREQUEST, BackToMenuRequest);

// #RVF : Document this
LUDEO_DECLARE_FUNC(LudeoNotificationId) ludeo_Session_AddNotifyRoomReady(LudeoHSession handle, const LudeoSessionAddNotifyRoomReadyParams* params, void* clientData, LudeoSessionRoomReadyCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_ADDNOTIFY(Session, SESSION, AddNotifyRoomReady, ADDNOTIFYROOMREADY, RoomReady);

// #ROOMS : Remove this once refactoring is done

/**
 * Signals the SDK to start the session as a ludeo player
 *
 * Prior to calling this, the client should set the game to the state required by this specific ludeo.
 * That can be done by using the function ludeo_Session_GetLudeo, which will give a DataReader instance that can be used to
 * retrieve the game state.
 *
 * Starting a Ludeo is a two step process (ludeo_Session_BeginLudeo and ludeo_Session_BeginGameplay)
 * If the call to this function succeeds, the client should then do the following:
 * - Use the provided DataWriter instance to write any initial state
 * - Call ludeo_Session_BeginGameplay and start the actual gameplay if that call succeeds
 * - Write any data to the DataWriter instance during gameplay
 * 
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback 
 * @param callback Callback called once the SDK and backend are ready to play the ludeo
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_BeginLudeo(LudeoHSession handle, const LudeoSessionBeginLudeoParams* params, void *clientData, LudeoSessionBeginLudeoCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, BeginLudeo, BEGINLUDEO);

// #ROOMS : Remove this once refactoring is done

/**
 * Signals the SDK to start actual gameplay
 * 
 * Starting a Creator or Ludeo Player run is a two step process.
 * The client calls either ludeo_Session_BeginCreator or ludeo_Sessoin_BeginLudeo, and if that call completes successfully, 
 * it should then call this function.
 *
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback 
 * @param callback Callback called once the SDK and backend are expecting the client to start actual gameplay.
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_BeginGameplay(LudeoHSession handle, const LudeoSessionBeginGameplayParams* params, void *clientData, LudeoSessionBeginGameplayCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, BeginGameplay, BEGINGAMEPLAY);

/**
 * Returns the mode the session is currently in.
 *
 * Note that calling this right after ludeo_Session_BeginCreator or ludeo_SessionBeginLudeo will return LudeoSession::None.
 * A session is only considered as switched to Creator or Player once the operation finishes successfully.
 *
 * The client should avoid making calls to ludeo_Session_BeginCreator, ludeo_Session_BeginLudeo or ludeo_Session_EndRun while
 * any of those is still in progress.
 * Doing so will fail with a LudeoResult::WrongState because the SDK knows there is one in progress, but because that call will
 * fail straight away, the callback will be called BEFORE the callback of the first call (which is still in progress). This can be
 * confusing for the client to handle.
 *
 * @param handle Session handle
 * @return What mode the session is in. If any error occurred (E.g: Invalid handle or SDK not initialized), it will return
 * LudeoSessionMode::None
 */
LUDEO_DECLARE_FUNC(LudeoSessionMode) ludeo_Session_GetMode(LudeoHSession handle);

/**
 * Marks an highlight
 *
 * This is a valid call only when the session is in creator mode. If called when the Session is not in valid state to receive this
 * call, the callback will return a LudeoResult::WrongState error code.
 *
 * The client should not make another call ludeo_Session_MarkHighlight until the previous one completes.
 *
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback
 * @param callback Callback to call once the operation completes
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_MarkHighlight(LudeoHSession handle, const LudeoSessionMarkHighlightParams* params, void* clientData, LudeoSessionMarkHighlightCallback callback );
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, MarkHighlight, MARKHIGHLIGHT);

/**
 * Ends a Creator or Ludeo player run
 *
 * This is a valid call only after a call to ludeo_Session_BeginCreator or ludeo_Session_BeginLudeo.
 * The error LudeoResult::WrongState is returned in the callback if called when in an invalid state.
 *
 * The client should avoid making calls to ludeo_Session_BeginCreator, ludeo_Session_BeginLudeo or ludeo_Session_EndRun while
 * any of those is still in progress.
 * Doing so will fail with a LudeoResult::WrongState because the SDK knows there is one in progress, but because that call will
 * fail straight away, the callback will be called BEFORE the callback of the first call (which is still in progress). This can be
 * confusing for the client to handle.
 *
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @param clientData Context to be passed to the callback
 * @param callback Callback to call once the operation completes
 */
LUDEO_DECLARE_FUNC(void) ludeo_Session_EndRun(LudeoHSession handle, const LudeoSessionEndRunParams* params, void* clientData, LudeoSessionEndRunCallback callback );
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Session, SESSION, EndRun, ENDRUN);


/**
 * Opens the ludeo gallery.
 *
 * This allows the client to programmatically open the ludeo gallery, instead of relying on the overlay button
 *
 * @param handle Session handle
 * @param params Structure with the function's parameters
 * @return LudeoResult::Success if the operation succeeded.
 */ 
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_Session_OpenGallery(LudeoHSession handle, const LudeoSessionOpenGalleryParams* params);
LUDEO_DEFINE_FUNC_TRAITS_2(Session, SESSION, OpenGallery, OPENGALLERY);

