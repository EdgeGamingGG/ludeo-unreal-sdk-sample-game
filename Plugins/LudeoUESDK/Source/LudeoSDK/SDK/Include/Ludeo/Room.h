// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/RoomTypes.h"

/**
 * Returns true or false depending if the handle is a valid handle.
 * This is useful for code that needs to quickly validate handles in a away that doesn't produce side effects. E.g: Debug code.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_Room_IsValid(LudeoHRoom handle);

/**
 * Returns the room's information
 * Check the LudeoRoomInfo struct for what information is available.
 * 
 * Once done with the information, the client needs to call ludeo_RoomInfo_Release to release the resources taken by
 * the structure.
 *
 * @param handle The instance to retrieve the information from
 * @param params Parameters to the functions
 * @param out Output parameter to receive the information.
 * @return If LudeoResult::Success, then the "out" parameter contains the information
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_Room_GetInfo(LudeoHRoom handle, const LudeoRoomGetInfoParams* params, LudeoRoomInfo** out);
LUDEO_DEFINE_FUNC_TRAITS_2(Room, ROOM, GetInfo, GETINFO);

/**
 * Release any resources hold by the specified structure
 *
 * @param info structure to release. If nulled, the call is a no-op.
 */
LUDEO_DECLARE_FUNC(void) ludeo_RoomInfo_Release(LudeoRoomInfo* info);

/**
 * Closes the specified room
 * This operation is asynchronous, and client should not attempt to open another room until this operation completes
 *
 * The room handle should not be used again after calling this. 
 *
 * @param handle Handle to the room
 * @param params Structure with any parameters the function needs
 * @param clientData Context that will be passed to callback
 * @param callback Callback to call once the operation completes
 */
LUDEO_DECLARE_FUNC(void) ludeo_Room_Close(LudeoHRoom handle, const LudeoRoomCloseParams* params, void* clientData, LudeoRoomCloseCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Room, ROOM, Close, CLOSE);


// #LC3636 : There is an edge case here. Internally (at the time of writing), we need to add one player at a time to the backend.
//  If one fails, I might have others that succeeded and then need to be removed if I want to consider the entire operation as
// failed. The way this will be resolved will have to take into consideration the RoomReady callback. There is a problem with this 
// entire thing of adding players that I haven't solved yet, because the SDK doesn't know when the developer is done adding players.
// Discussion at : https://ludeos-workspace.slack.com/archives/C06DGAFV85P/p1705671539449619

/**
 * Adds a player to a room.
 *
 * See the LudeoRoomAddPlayerParams struct for more details.
 *
 * @param handle Handle to the room
 * @param params Structure with any parameters the function needs
 * @param clientData Context that will be passed to callback
 * @param callback Callback to call once the operation completes
 */
LUDEO_DECLARE_FUNC(void) ludeo_Room_AddPlayer(LudeoHRoom handle, const LudeoRoomAddPlayerParams* params, void* clientData, LudeoRoomAddPlayerCallback callback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Room, ROOM, AddPlayer, ADDPLAYER);

/**
 * Removes a player from a room
 *
 * Any existing GameplaySession handle associated with the players will be automaticaly invalidated.
 *
 * @param handle Handle to the room
 * @param params Structure with any parameters the function needs
 * @param clientData Context that will be passed to callback
 * @param callback Callback to call once the operation completes
 */
LUDEO_DECLARE_FUNC(void) ludeo_Room_RemovePlayer(LudeoHRoom handle, const LudeoRoomRemovePlayerParams* params, void *clientData, LudeoRoomRemovePlayerCallback);
LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(Room, ROOM, RemovePlayer, REMOVEPLAYER);

