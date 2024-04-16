// Copyright 2023 Ludeo. All rights reserved.

/**
 * LudeoDataWriter provides the interface for the client to send gameplay data to the SDK.
 *
 * The client needs to send gameplay relevant data to the backend during both Creator and Ludeo runs.
 *
 * - During a Creator run, the client needs to report gameplay data so the backend then has the required data to allow creating
 *   Ludeos
 * - During a Ludeo run, the client needs to report it so the backend calculates scoring for said Ludeo.
 *
 * A DataWriter instance is tied to the run that it originated from, and is invalidated as soon as the run finishes.
 * As-in, an instance is given to the client when starting a Creator or Player run so the client can report data. Once the run
 * finishes, the handle is invalidated automatically
 *
 * Data is not sent necessarily every tick. The real frequency will depend on the SDK internal logic, any relevant backend
 * settings, and settings specified by the client.
 */
#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/DataWriterTypes.h"

/**
 * Returns true or false depending if the handle is a valid handle.
 * This is useful for code that needs to quickly validate handles in a away that doesn't produce side effects. E.g: Debug code.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_IsValid(LudeoHDataWriter handle);

// #RVF I'm not sure specifying a global send frequency is a smart enough approach. What we end up doing might depend on if we
// implement the snapshot thing we discussed about. Some keys might need more frequent sends than others, and this might be game
// specific.

/**
 * Allows tweaking the data sending rules. E.g, how often to send data.
 * The SDK assumes sensible defaults.
 *
 * One thing to note is that data sending is triggered only as the result of a ludeo_Tick call, therefore the true frequency of
 * the sends doesn't depend solely on the frequency the client specifies.
 * When the client calls ludeo_Tick, the SDK checks the total time elapsed since the last send, and compares it against the desired
 * send frequency. If greater or equal, then it sends any pending data.
 *
 * @param handle The instance handle
 * @param parms Parameters to the function
 * @return LudeoResult::Success on success.
 */ 
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataWriter_SetSendSettings(LudeoHDataWriter handle, const LudeoDataWriterSetSendSettingsParams* params);
LUDEO_DEFINE_FUNC_TRAITS_2(DataWriter, DATAWRITER, SetSendSettings, SETSENDSETTINGS);

/**
 * Create a new object to track
 * 
 * This function can be used for two purposes:
 * - Create an new object
 * - Or validate that the client is properly setup (when playing a ludeo).
 *
 * When playing a ludeo, the SDK will internally create any objects that exist in the ludeo data and the client is
 * expected to query the ludeo data for the existing object ids (and objecType) and assign those object ids to the right instances
 * it created. The output parameter "outObjectId" also functions as an input parameter to specify what behaviour to use.
 *
 * @param handle DataWriter to use
 * @param params Function parameters
 * @param outObjectId
 *	- On entry it should point to a LUDEO_INVALID_OBJECTID value or a valid object id. If LUDEO_INVALID_OBJECTID, the SDK will
 * create a new object, and on exit it will contain the new object's id.
 *	- If on entry it points to something else other than a LUDEO_INVALID_OBJECTID, the SDK validates if that is an existing object
 *	id and it matches the objectType specified in params. If it doesn't match it returns an error.
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataWriter_CreateObject(LudeoHDataWriter handle, const LudeoDataWriterCreateObjectParams* params, LudeoObjectId* outObjectId);
LUDEO_DEFINE_FUNC_TRAITS_2(DataWriter, DATAWRITER, CreateObject, CREATEOBJECT);

/**
 * Destroyed the specified object
 *
 * If the call succeeds, the client shouldn not attempt to make use of this object id again
 *
 * @param handle DataWriter to use
 * @param params Function parameters
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataWriter_DestroyObject(LudeoHDataWriter handle, const LudeoDataWriterDestroyObjectParams* params);
LUDEO_DEFINE_FUNC_TRAITS_2(DataWriter, DATAWRITER, DestroyObject, DESTROYOBJECT);

/**
 * Sets the DataWriter instance to use for any further calls to DataWriter API functions that don't explicitly ask for a handle.
 *
 * Since the client will be calling DataWriter possibly hundreds/thousands of times per frame, this function allows the SDK
 * to do the necessary API checks once (handle checking, internal lookups, etc), cache what it needs, and thus decrease the
 * overhead of the ludeo_DataWriter_SetXXX calls themselves.
 * 
 * If for whatever reason the client needs to call this function for every single ludeo_DataWriter_SetXXX call, then it should
 * cache what was the last handle passed to SetCurrent, and only make the call if necessary.
 *
 * E.g, considering the client has a generic write function that accepts a DataWriter handle and sets an attribute:
 *
 * --------------------------------------
 *	template<typename T>
 *	bool genericWrite(LudeoHDataWriter dataWriter, const char* attributeName, const T& value)
 *	{
 *		static LudeoHDataWriter current = nullptr;
 * 
 *		if (current != dataWriter)
 *		{
 *			ludeo_DataWriter_SetCurrent(dataWriter);
 *			current = dataWriter;
 *		}
 * 
 *		return ludeo_DataWriter_Set(attributeName, value) == LUDEO_TRUE ? true : false;
 *	}
 * --------------------------------------
 *
 *
 * One IMPORTANT details is to compare the handles NOT an object pointer. For example, the following approach has a bug that will
 * only manifest if the client handle wrapper gets destroyed and another one created in the same memory address:
 *
 * --------------------------------------
 *	// Some wrapper the client has for the handle
 *	struct LudeoDataWriterWrapper
 *	{
 *		LudeoHDataWriter handle;
 *		... Other data and function members ...
 *	};
 *	
 *	template<typename T>
 *	bool genericWrite(LudeoDataWriterWrapper& dataWriter, const char* attributeName, const T& value)
 *	{
 *		static LudeoDataWriterWrapper* current = nullptr;
 *	
 *		if (current != &dataWriter) // Possible bug. "current" and "dataWriter" might be the same pointer, but different handles
 *		{
 *			ludeo_DataWriter_SetCurrent(dataWriter);
 *			current = &dataWriter;
 *		}
 *	
 *		return ludeo_DataWriter_Set(attributeName, value) == LUDEO_TRUE ? true : false;
 *	}
 * --------------------------------------
 * 
 * @param handle Handle to the instance to set, or nullptr to unset
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataWriter_SetCurrent(LudeoHDataWriter handle);

/**
 * Pushes an object to the DataWriter context stack
 * Any ludeo_DataWriter_XXX calls to write data will act on this object, until ludeo_DataWriter_LeaveObject is called.
 * 
 * @param objectId Object to use.
 * @return True if successfully entered the object (the object exists), or False if the object doesn't exist.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_EnterObject(LudeoObjectId objectId);

/**
 * Leaves the current object.
 * This should only be called after a successful call to ludeo_DataWriter_EnterObject.
 *
 * @return True if there were no errors detected such as not being inside an object. The only reason this can return False is if
 * the client has a mismatch in the EnterObject/LeaveObject calls.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_LeaveObject();

// #ROOMS : Implement these
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_EnterComponent(const char* name);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_LeaveComponent();

/**
 * Binds a player to the current object.
 *
 * This should only be called when inside an object. 
 * It applies to the object itself and is not affected by the actual full context stack level. As-in, once inside an object,
 * this can be called anywhere in the context stack.
 *
 * Repeatedly changing the player binding on a given object has the potential to increase bandwidth (and cpu) usage.
 * This is because any data written to the DataWriter is associated with the binding (either if set or not), so once a binding is
 * changed, the SDK needs to forcibly send any pending data that object has.
 *
 * @param playerId
 *	If specified and not an empty string, it will bind the current object to that player.
 *	If nullptr or an empty string it will remove the current binding.
 *	An object only allows one player to be bound to it at anytime, so if the client wishes to change the binding from one player
 *  to another, there is no need for an intermediate call with null (to unbind). It can simply specify the player to switch to.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetPlayerBinding(const char* playerId);

// #ROOMS : Implement these
/**
 * Writes the specified attribute to the current context.
 * @param value Value to write
 * @param value Value to write
 * @return True if the attribute was found and is of the right type, in which case "outValue" will contain the value.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetBool(const char* attributeName, LudeoBool value);
// If using C++, then overload to allow using "bool" directly
#if __cplusplus
	inline LudeoBool ludeo_DataWriter_SetBool(const char* attributeName, bool value)
	{
		return ludeo_DataWriter_SetBool(attributeName, LudeoBool(value));
	}
#endif
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetInt8(const char* attributeName, int8_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetUInt8(const char* attributeName, uint8_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetInt16(const char* attributeName, int16_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetUInt16(const char* attributeName, uint16_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetInt32(const char* attributeName, int32_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetUInt32(const char* attributeName, uint32_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetInt64(const char* attributeName, int64_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetUInt64(const char* attributeName, uint64_t value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetFloat(const char* attributeName, float value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetDouble(const char* attributeName, double value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetVec3Float(const char* attributeName, const float* value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetVec4Float(const char* attributeName, const float* value);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataWriter_SetString(const char* attributeName, const char* value);


/**
 * If using C++, then we can simplify how data is set by using templates
 *
 * The client can add overrides for it's own types, like string classes.
 */
#if __cplusplus

	#if __has_include(<type_traits>)
		#include <type_traits>
	#endif

	template<typename T>
	LudeoBool ludeo_DataWriter_Set(const char* attributeName, T value)
	{
		if constexpr(std::is_same_v<T, int8_t>)
		{
			return ludeo_DataWriter_SetInt8(attributeName, value);
		}
		else if constexpr(std::is_same_v<T, uint8_t>)
		{
			return ludeo_DataWriter_SetUInt8(attributeName, value);
		}

		else if constexpr(std::is_same_v<T, int16_t>)
		{
			return ludeo_DataWriter_SetInt16(attributeName, value);
		}
		else if constexpr(std::is_same_v<T, uint16_t>)
		{
			return ludeo_DataWriter_SetUInt16(attributeName, value);
		}

		else if constexpr(std::is_same_v<T, int32_t>)
		{
			return ludeo_DataWriter_SetInt32(attributeName, value);
		}
		else if constexpr(std::is_same_v<T, uint32_t>)
		{
			return ludeo_DataWriter_SetUInt32(attributeName, value);
		}

		else if constexpr(std::is_same_v<T, int64_t>)
		{
			return ludeo_DataWriter_SetInt64(attributeName, value);
		}
		else if constexpr(std::is_same_v<T, uint64_t>)
		{
			return ludeo_DataWriter_SetUInt64(attributeName, value);
		}

		else if constexpr(std::is_same_v<T, bool>)
		{
			return ludeo_DataWriter_SetBool(attributeName, value);
		}
		else if constexpr(std::is_same_v<T, float>)
		{
			return ludeo_DataWriter_SetFloat(attributeName, value);
		}
		else if constexpr(std::is_same_v<T, double>)
		{
			return ludeo_DataWriter_SetDouble(attributeName, value);
		}
		else if constexpr(std::is_same_v<T, const char*>)
		{
			return ludeo_DataWriter_SetString(attributeName, value);
		}
		else
		{
			static_assert(std::is_same_v<T, void> == true);
			return LUDEO_FALSE;
		}
	}

#endif

/*
 * Sends a game action to the backend
 *
 * Actions have no parameters. They represent a single thing such as "Kill", "HeadShot", etc.
 *
 * @param handle DataWriter to use
 * @param params Function parameters
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataWriter_SendAction(LudeoHDataWriter handle, const LudeoDataWriterSendActionParams* params);
LUDEO_DEFINE_FUNC_TRAITS_2(DataWriter, DATAWRITER, SendAction, SENDACTION);

