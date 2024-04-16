// Copyright 2023 Ludeo. All rights reserved.

/**
 * This file contains non-essential utilities
 */

#pragma once

#include "Ludeo/Common.h"
#include "Ludeo/InitTypes.h"
#include "Ludeo/SessionTypes.h"


/**
 * By default, this file doesn't introduce the use of std::string. If you wish to make use of the to_string functions, you need
 * to define LUDEOSDK_TO_STRING=1
 */

#if __cplusplus
	#if LUDEOSDK_BUILDING
		#define LUDEOSDK_TO_STRING 1
	#else
		#ifndef LUDEOSDK_TO_STRING
			#define LUDEOSDK_TO_STRING 0
		#endif
	#endif

#else
	#if defined(LUDEOSDK_TO_STRING) && LUDEOSDK_TO_STRING
		#pragma warning "Compiling as C. Forcing LUDEOSDK_TO_STRING to 0"
		#undef LUDEOSDK_TO_STRING
		#define LUDEOSDK_TO_STRING 0
	#else
		#define LUDEOSDK_TO_STRING 0
	#endif

#endif

/**
 * If building unit tests (LUDEOSDK_UNITTESTS is 1), then we introduce Catch2 helpers
 */
#if LUDEOSDK_UNITTESTS && LUDEOSDK_TO_STRING
	#define LUDEO_IMPL_UNITTEST_HELPERS(Enum)                                    \
		namespace Catch                                                          \
		{                                                                        \
		template <>                                                              \
		struct StringMaker<Ludeo##Enum>                                          \
		{                                                                        \
			static std::string convert(Ludeo##Enum const& value)                 \
			{                                                                    \
				return std::string("Ludeo") + #Enum + "::" + ::to_string(value); \
			}                                                                    \
		};                                                                       \
		}
#else
	#define LUDEO_IMPL_UNITTEST_HELPERS(Enum)
#endif

/**
 * By default the C++ helpers use std::string by default. You can override that by defining LUDEOSDK_TO_STRING_TYPE.
 * eg: LUDEOSDK_TO_STRING_TYPE=YourOwnStringClass
 */
#if LUDEOSDK_TO_STRING
	#if defined (LUDEOSDK_TO_STRING_TYPE)
		// Using a custom type
		using LudeoString = LUDEOSDK_TO_STRING_TYPE;
	#else
		#include <string>
		using LudeoString = std::string;
	#endif

	#define LUDEO_IMPL_TO_STRING_CPP(Name) \
		inline const char* to_cstring(Ludeo##Name value) { return ludeo_##Name##ToString(value); } \
		inline LudeoString to_string(Ludeo##Name value) { return ludeo_##Name##ToString(value); }
		
#else
	#define LUDEO_IMPL_TO_STRING_CPP(Name) 
#endif

#define LUDEO_IMPL_ENUM_HELPERS(Name)                                          \
	LUDEO_DECLARE_FUNC(const char*) ludeo_##Name##ToString(Ludeo##Name value); \
	LUDEO_IMPL_TO_STRING_CPP(Name);                                            \
	LUDEO_IMPL_UNITTEST_HELPERS(Name);

/**
 * Declare define "to string" helpers for enums
 */
LUDEO_IMPL_ENUM_HELPERS(Result)
LUDEO_IMPL_ENUM_HELPERS(LogLevel)
LUDEO_IMPL_ENUM_HELPERS(LogCategory)
LUDEO_IMPL_ENUM_HELPERS(AuthType)
LUDEO_IMPL_ENUM_HELPERS(DataType)
LUDEO_IMPL_ENUM_HELPERS(SessionMode)



#if __cplusplus
/**
 * Utility function to create a parameters struct and initialize apiVersion to the right value.
 * In addition to setting apiVersion, it zeroes all other fields.
 *
 * This can avoid bugs such as using a wrong API_LATEST value (e.g: copy pasting bugs) or not zeroing fields
 *
 * Can be used as e.g:
 * 
 * auto params = Ludeo::create<LudeoSessionCreateParams>();
 * // Set other fields as required...
 * params.platformUrl = ...;
 * // ... call ludeo_Session_Create with `params`
 *
 */
namespace Ludeo
{
	template<typename ParamsType>
	ParamsType create()
	{
		// Note the {}. That's for zero initialization: https://en.cppreference.com/w/cpp/language/zero_initialization
		ParamsType params{};
		params.apiVersion = Ludeo::ApiFuncTraits<ParamsType>::latestApiVersion;
		return params;
	}
} // namespace Ludeo
#endif


/**
 * 128-bit UUID
 */
LUDEO_STRUCT(LudeoUUID, (
	uint8_t data[16]; 
));

/**
 * Generates a UUID
 *
 * This is provided simply as a convenient cross-platform way of creating UUIDs.
 */
LUDEO_DECLARE_FUNC(LudeoUUID) ludeo_CreateUUID();

/**
 * Similar to ludeo_CreateUUID, but returns a string representation 
 *
 * The resulting string is a 36 characters long string (e.g: "550e8400-e29b-41d4-a716-446655440000" ).
 * The specified buffer needs to be at least 37 bytes (36 + null-terminating character)
 *
 * @param buf Buffer where to write the null-terminated string
 * @param bufSize size of the provided buffer in bytes. The function expects a buffer of size 37. This parameter is specified just a
 * safety. If bufSize is <37, LudeoResult::InvalidParameters is returned.
 */
LUDEO_DECLARE_FUNC(LudeoResult) ludeo_CreateUUIDString(char* buf, int32_t bufSize);

