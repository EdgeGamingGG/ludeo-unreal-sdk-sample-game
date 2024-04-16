// Copyright 2023 Ludeo. All rights reserved.

/**
 * 
 * The DataReader interface encapsulates the concept of ludeo data. As-in, it contains all the information related to a
 * specific ludeo, such as the necessary data to restore the game state. 
 *
 * The lifetime of a DataReader instance is tied to the Session lifetime but not to a specific run. When a session is destroyed,
 * any DataReader handles created as part of that Session are invalidated, although the client should call
 * ludeo_DataReader_Release when it has no further need for it, to release resources.
 */

#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/DataReaderTypes.h"

/**
 * Returns true or false depending if the handle is a valid handle.
 * This is useful for code that needs to quickly validate handles in a away that doesn't produce side effects. E.g: Debug code.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_IsValid(LudeoHDataReader handle);

/**
 * Releases the specified DataReader instance 
 * @param handle The DataReader's handle to release.
 * Any attempt to use the handle after this call will fail.
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataReader_Release(LudeoHDataReader handle);

// #ROOMS : This is not implemented yet.
/**
 * Returns information related to this ludeo.
 * Check the LudeoDataReaderInfo struct for what information is available
 *
 * Once done with the information, the client needs to call ludeo_DataReaderInfo_Release to release the resources taken by
 * the structure.
 *
 * @param handle The DataReader instance to retrieve the ludeoId from.
 * @param params Parameters to the function
 * @param out Output parameter used to receive the LudeoDataReaderInfo structure
 * @return If LudeoResult::Success, then the "out" parameter will contain a pointer to the structure. If an error, then "out" will
 *		contain null
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataReader_GetInfo(LudeoHDataReader handle, const LudeoDataReaderGetInfoParams* params, LudeoDataReaderInfo** out);
LUDEO_DEFINE_FUNC_TRAITS_2(DataReader, DATAREADER, GetInfo, GETINFO);

// #ROOMS : This is not implemented yet.
/**
 * Release any resources hold by the specified structure
 *
 * @param info structure to release. If nulled, the call is a no-op.
 */
LUDEO_DECLARE_FUNC(void) ludeo_DataReaderInfo_Release(LudeoDataReaderInfo* info);

/**
 * Retrieve information about all the existing objects in the ludeo data.
 * The client can use this information to create all the required object instances.
 * The client is required to maintain the right objectId/objectType mappings to its object instances. E.g: if using the SDK from
 * C++, when playing as a creator, the client will most likely create instances of several classes, and for every relevant instance
 * it will call ludeo_DataWriter_CreateObject to register the object (with the right "objectType"). When restoring the data to
 * play a ludeo, the client is expected to create the right instances and set their objectId to the one supplied by this API.
 *
 * Once done with the information the client needs to call ludeo_ObjectsInfo_Release to release the resources taken by the structure.
 *
 * @param handle The DataReader instance to get the information from
 * @param params Parameters to the function
 * @param out Information about all the objects. Once the client is done using this, it should call ludeo_ObjectsInfo_Release to deallocate the structure
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataReader_GetObjectsInfo(LudeoHDataReader handle, const LudeoDataReaderGetObjectsInfoParams* params, LudeoObjectsInfo** out);
LUDEO_DEFINE_FUNC_TRAITS_2(DataReader, DATAREADER, GetObjectsInfo, GETOBJECTSINFO);

/**
 * Release the resources taken by a LudeoObjectsInfo structure
 */
LUDEO_DECLARE_FUNC(void) ludeo_ObjectsInfo_Release(LudeoObjectsInfo* info);

/**
 * Sets the DataReader instance to use for any further calls to DataReader API functions that don't explicitly ask for a handle.
 * This allows the client to have less verbose code when reading data.
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_DataReader_SetCurrent(LudeoHDataReader handle);

/**
 * Pushes an object to the DataReader context stack
 * Any ludeo_DataReader_XXX calls to read data will act on this object, until ludeo_DataReader_LeaveObject is called.
 *
 * Typically a client will first make a call to ludeo_DataReader_GetObjectsInfo to read back all the object ids and their types.
 * Once the client has instantiated whatever it needs, it can then use those object ids to read back the data into the instances.
 *
 * @param objectId Object to use.
 * @return True if successfully entered the object (the object exists), or False if the object doesn't exist.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_EnterObject(LudeoObjectId objectId);

/**
 * Leaves the current object.
 * This should only be called after a successful call to ludeo_DataReader_EnterObject.
 *
 * @return True if there were no errors detected such as not being inside an object. The only reason this can return False is if
 * the client has a mismatch in the EnterObject/LeaveObject calls.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_LeaveObject();

// #ROOMS : Implement these
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_EnterComponent(const char* name);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_LeaveComponent();

/**
 * Looks for the specified attribute in the current context and returns its size in bytes.
 *
 * This allows the client to query the size of attributes for which it needs to pre-allocate memory. E.g, when reading back a
 * dynamic string.
 *
 * @param outSize Size of the attribute in bytes. If the attribute is a string, this is the size of the string in bytes, excluding
 *  the null character. E.g: If the string is empty, this will be 0.
 * @return True if the attribute was found at the current level, in which case outSize will contain the size.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetSize(const char* attributeName, uint32_t* outSize);

/**
 * Reads the specified attribute from the current context.
 *
 * @param attributeName Attribute to read
 * @param outValue On exit it will contain the attribute value
 * @return True if the attribute was found and is of the right type, in which case "outValue" will contain the value.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetBool  (const char* attributeName, LudeoBool* outValue);
// If using C++, then overload to allow using "bool" directly
#if __cplusplus
	inline LudeoBool ludeo_DataReader_GetBool(const char* attributeName, bool* value)
	{
		LudeoBool tmpValue;
		LudeoBool ret = ludeo_DataReader_GetBool(attributeName, &tmpValue);
		if (ret == LUDEO_TRUE)
		{
			*value = (tmpValue == LUDEO_TRUE) ? true : false;
		}

		return ret;
	}
#endif
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetInt8  (const char* attributeName, int8_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetUInt8 (const char* attributeName, uint8_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetInt16 (const char* attributeName, int16_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetUInt16(const char* attributeName, uint16_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetInt32 (const char* attributeName, int32_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetUInt32 (const char* attributeName, uint32_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetInt64 (const char* attributeName, int64_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetUInt64 (const char* attributeName, uint64_t* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetFloat (const char* attributeName, float* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetDouble(const char* attributeName, double* outValue);
/**
 * Gets the specified string attribute.
 * @param attributeName attribute to get
 * @param outValue where to copy the string to
 * 
 * IMPORTANT: The NULL character is NOT copied, and it's the client's responsability to correctly terminate the string.
 * When reading strings, the client should first do a call to ludeo_DataReader_GetSize to get the string size and make sure the
 * buffer provided to ludeo_DataReader_GetString is big enough. After the ludeo_DataReader_SetString, the client can then null
 * terminate the string since it knows the size.
 */
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetString(const char* attributeName, char* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetVec3Float(const char* attributeName, float* outValue);
LUDEO_DECLARE_FUNC(LudeoBool) ludeo_DataReader_GetVec4Float(const char* attributeName, float* outValue);

/**
 * If using C++, then we can simplify how data is read by using templates
 * 
 * The client can add overrides for it's own types, like string classes.
 */
#if __cplusplus

	#if __has_include(<type_traits>)
		#include <type_traits>
	#endif

	template<typename T>
	LudeoBool ludeo_DataReader_Get(const char* attributeName, T& value)
	{
		if constexpr(std::is_same_v<T, bool>)
		{
			return ludeo_DataReader_GetBool(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, int8_t>)
		{
			return ludeo_DataReader_GetInt8(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, uint8_t>)
		{
			return ludeo_DataReader_GetUInt8(attributeName, &value);
		}

		else if constexpr(std::is_same_v<T, int16_t>)
		{
			return ludeo_DataReader_GetInt16(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, uint16_t>)
		{
			return ludeo_DataReader_GetUInt16(attributeName, &value);
		}

		else if constexpr(std::is_same_v<T, int32_t>)
		{
			return ludeo_DataReader_GetInt32(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, uint32_t>)
		{
			return ludeo_DataReader_GetUInt32(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, int64_t>)
		{
			return ludeo_DataReader_GetInt64(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, uint64_t>)
		{
			return ludeo_DataReader_GetUInt64(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, float>)
		{
			return ludeo_DataReader_GetFloat(attributeName, &value);
		}
		else if constexpr(std::is_same_v<T, double>)
		{
			return ludeo_DataReader_GetDouble(attributeName, &value);
		}
		else
		{
			static_assert(std::is_same_v<T, void> == true);
			return LUDEO_FALSE;
		}
	}

	inline LudeoBool ludeo_DataReader_Get(const char* attributeName, char* value)
	{
		return ludeo_DataReader_GetString(attributeName, value);
	}

#endif
