// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "stdint.h"

// Set the LUDEOSDK_<PLATFORM_NAME> macro
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define LUDEOSDK_WINDOWS 1
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE && TARGET_OS_SIMULATOR
		// define something for simulator
		// (although, checking for TARGET_OS_IPHONE should not be required).
		#error Unknown/unsupported platform
	#elif TARGET_OS_IPHONE && TARGET_OS_MACCATALYST
		// define something for Mac's Catalyst
		#error Unknown/unsupported platform
	#elif TARGET_OS_IPHONE
		// define something for iphone  
		#error Unknown/unsupported platform
	#else
		#define LUDEOSDK_MACOSX 1
	#endif
#elif __linux__
	#define LUDEOSDK_LINUX 1
#else
	#error Unknown/unsupported platform
#endif

#ifndef LUDEOSDK_WINDOWS
	#define LUDEOSDK_WINDOWS 0
#endif

#ifndef LUDEOSDK_MACOSX
	#define LUDEOSDK_MACOSX 0
#endif

#ifndef LUDEOSDK_LINUX
	#define LUDEOSDK_LINUX 0
#endif


#if defined(_WIN32) || defined(_WIN64)
	#if defined(_WIN64)
		#define LUDEOSDK_64BITS 1
		#define LUDEOSDK_32BITS 0
	#else
		#define LUDEOSDK_64BITS 0
		#define LUDEOSDK_32BITS 1
	#endif
#elif defined __GNUC__
	#if defined(__x86_64__) || defined(__ppc64__) || defined(__arm64__)
		#define LUDEOSDK_64BITS 1
		#define LUDEOSDK_32BITS 0
	#else
		#define LUDEOSDK_64BITS 0
		#define LUDEOSDK_32BITS 1
	#endif
#else
	#error Unknown compiler
#endif

#if __cplusplus
	#include <functional>
#endif

#ifndef LUDEOSDK_BUILDING
	#define LUDEOSDK_BUILDING 0
#endif

#ifndef LUDEOSDK_UNITTESTS
	#define LUDEOSDK_UNITTESTS 0
#endif

#ifndef LUDEOSDK_BUILDING_FOR_UNITTESTS
	#define LUDEOSDK_BUILDING_FOR_UNITTESTS 0
#endif


#if LUDEOSDK_BUILDING_FOR_UNITTESTS
	#if defined _WIN32
		#define LUDEOSDK_API
	#elif __GNUC__ >= 4
		#define LUDEOSDK_API __attribute__((__visibility__("default")))
	#else
		#error Unexpected platform?
	#endif
#elif LUDEOSDK_BUILDING
	#if defined _WIN32
		#define LUDEOSDK_API __declspec(dllexport)
	#elif __GNUC__ >= 4
		#define LUDEOSDK_API __attribute__((__visibility__("default")))
	#else
		#error Unexpected platform?
	#endif
#else
	#if defined _WIN32
		#define LUDEOSDK_API __declspec(dllimport)
	#elif __GNUC__ >= 4
		#define LUDEOSDK_API __attribute__((__visibility__("default")))
	#else
		#error Unexpected platform?
	#endif
#endif

#if defined(_WIN32) && (defined(__i386) || defined(_M_IX86))
	#define LUDEO_CALL __stdcall
#else
	#define LUDEO_CALL
#endif

#ifdef __cplusplus
	#define EXTERN_C extern "C"
#else
	#define EXTERN_C
#endif

#define LUDEO_DECLARE_FUNC(ReturnType) EXTERN_C LUDEOSDK_API ReturnType LUDEO_CALL
#define LUDEO_IMPL_FUNC(ReturnType) EXTERN_C ReturnType LUDEO_CALL

#ifdef __cplusplus
	// std::function style signature for a callback
	#define LUDEO_DECLARE_CALLBACK_CPP(CallbackName, ...) \
		using LUDEO_CONCATENATE(CallbackName, Cpp) = std::function<void(__VA_ARGS__)>;
#else
	#define LUDEO_DECLARE_CALLBACK_CPP(CallbackName, ...)
#endif


#define LUDEO_DECLARE_CALLBACK(CallbackName, ...)                  \
	EXTERN_C typedef void(LUDEO_CALL * CallbackName)(__VA_ARGS__); \
	LUDEO_DECLARE_CALLBACK_CPP(CallbackName, __VA_ARGS__)

#define LUDEO_DECLARE_CALLBACK_RETVALUE(ReturnType, CallbackName, ...) \
	EXTERN_C typedef ReturnType(LUDEO_CALL* CallbackName)(__VA_ARGS__)

#define LUDEO_PASTE(...) __VA_ARGS__
#define LUDEO_STRUCT(StructName, StructDef)  \
	EXTERN_C typedef struct _tag##StructName \
	{                                        \
		LUDEO_PASTE StructDef                \
	} StructName

//
// Utility macros to concatenate things
//
#define LUDEO_CONCATENATE_PASTER(s1,s2) s1 ## s2
#define LUDEO_CONCATENATE(s1,s2) LUDEO_CONCATENATE_PASTER(s1,s2)

//
// Helpers to make things easier to use from C++
//
#if __cplusplus

	namespace Ludeo
	{
		template<typename T>
		struct ApiFuncTraits
		{
		};
	}

	// Defines traits for a function that doesn't belong to an interface
	#define LUDEO_DEFINE_FUNC_TRAITS(inFuncName, inFuncNameU)                             \
	    namespace Ludeo                                                                   \
	    {                                                                                 \
	    template <>                                                                       \
	    struct ApiFuncTraits<Ludeo##inFuncName##Params>                                   \
	    {                                                                                 \
		    static constexpr int32_t latestApiVersion = LUDEO_##inFuncNameU##_API_LATEST; \
		    using ParamsType = Ludeo##inFuncName##Params;                                 \
		    using FuncType = decltype(&ludeo_##inFuncName);                               \
		    static FuncType func()                                                        \
		    {                                                                             \
			    return &ludeo_##inFuncName;                                               \
		    }                                                                             \
	    };                                                                                \
	    }

	// Defines traits for a function that belong to an interface
	#define LUDEO_DEFINE_FUNC_TRAITS_2(inInterfaceName, inInterfaceNameU, inFuncName, inFuncNameU)             \
	    namespace Ludeo                                                                                        \
	    {                                                                                                      \
	    template <>                                                                                            \
	    struct ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##Params>                                       \
	    {                                                                                                      \
		    static constexpr int32_t latestApiVersion = LUDEO_##inInterfaceNameU##_##inFuncNameU##_API_LATEST; \
		    using ParamsType = Ludeo##inInterfaceName##inFuncName##Params;                                     \
		    using FuncType = decltype(&ludeo_##inInterfaceName##_##inFuncName);                                \
			static constexpr const char* funcName()                                                            \
			{                                                                                                  \
				return "ludeo_" #inInterfaceName "_" #inFuncName;                                              \
			}                                                                                                  \
		    static FuncType func()                                                                             \
		    {                                                                                                  \
			    return &ludeo_##inInterfaceName##_##inFuncName;                                                \
		    }                                                                                                  \
	    };                                                                                                     \
	    }

	// Defines traits for a function that doesn't belong to an interface and that has a callback
	#define LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK(inFuncName, inFuncNameU)                                           \
		namespace Ludeo                                                                                              \
		{                                                                                                            \
			template <>                                                                                              \
			struct ApiFuncTraits<Ludeo##inFuncName##Params>                                                          \
			{                                                                                                        \
				static constexpr int32_t latestApiVersion = LUDEO_##inFuncNameU##_##API_LATEST;                      \
				using ParamsType = Ludeo##inFuncName##Params;                                                        \
				static constexpr const char* funcName()                                                              \
				{                                                                                                    \
					return "ludeo_" #inFuncName;                                                                     \
				}                                                                                                    \
				using FuncType = decltype(&ludeo_##inFuncName);                                                      \
				static FuncType func()                                                                               \
				{                                                                                                    \
					return &ludeo_##inFuncName;                                                                      \
				}                                                                                                    \
				using CallbackType = Ludeo##inFuncName##Callback;                                                    \
				using CallbackTypeCpp = Ludeo##inFuncName##CallbackCpp;                                              \
				using CallbackParamsType = Ludeo##inFuncName##CallbackParams;                                        \
			};                                                                                                       \
			                                                                                                         \
			template <>                                                                                              \
			struct ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##CallbackParams>                                 \
				: public ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##Params>                                   \
			{                                                                                                        \
			};                                                                                                       \
		}

	// Defines traits for a function that belongs to an interface and that has a callback
	#define LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(inInterfaceName, inInterfaceNameU, inFuncName, inFuncNameU)      \
		namespace Ludeo                                                                                              \
		{                                                                                                            \
			template <>                                                                                              \
			struct ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##Params>                                         \
			{                                                                                                        \
				static constexpr int32_t latestApiVersion = LUDEO_##inInterfaceNameU##_##inFuncNameU##_##API_LATEST; \
				using ParamsType = Ludeo##inInterfaceName##inFuncName##Params;                                       \
				static constexpr const char* funcName()                                                              \
				{                                                                                                    \
					return "ludeo_" #inInterfaceName "_" #inFuncName;                                                \
				}                                                                                                    \
				using FuncType = decltype(&ludeo_##inInterfaceName##_##inFuncName);                                  \
				static FuncType func()                                                                               \
				{                                                                                                    \
					return &ludeo_##inInterfaceName##_##inFuncName;                                                  \
				}                                                                                                    \
				using CallbackType = Ludeo##inInterfaceName##inFuncName##Callback;                                   \
				using CallbackTypeCpp = Ludeo##inInterfaceName##inFuncName##CallbackCpp;                             \
				using CallbackParamsType = Ludeo##inInterfaceName##inFuncName##CallbackParams;                       \
			};                                                                                                       \
																													 \
			template <>                                                                                              \
			struct ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##CallbackParams>                                 \
				: public ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##Params>                                   \
			{                                                                                                        \
			};                                                                                                       \
		}

	// Defines traits for a "AddNotify" function
	#define LUDEO_DEFINE_FUNC_TRAITS_ADDNOTIFY(inInterfaceName, inInterfaceNameU, inFuncName, inFuncNameU, CallbackName)      \
		namespace Ludeo                                                                                                       \
		{                                                                                                                     \
			template <>                                                                                                       \
			struct ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##Params>                                                  \
			{                                                                                                                 \
				static constexpr int32_t latestApiVersion = LUDEO_##inInterfaceNameU##_##inFuncNameU##_##API_LATEST;          \
				using ParamsType = Ludeo##inInterfaceName##inFuncName##Params;                                                \
				static constexpr const char* funcName()                                                                       \
				{                                                                                                             \
					return "ludeo_" #inInterfaceName "_" #inFuncName;                                                         \
				}                                                                                                             \
				using FuncType = decltype(&ludeo_##inInterfaceName##_##inFuncName);                                           \
				static FuncType func()                                                                                        \
				{                                                                                                             \
					return &ludeo_##inInterfaceName##_##inFuncName;                                                           \
				}                                                                                                             \
				using CallbackType = Ludeo##inInterfaceName##CallbackName##Callback;                                          \
				using CallbackTypeCpp = Ludeo##inInterfaceName##CallbackName##CallbackCpp;                                    \
				using CallbackParamsType = Ludeo##inInterfaceName##CallbackName##CallbackParams;                              \
			};                                                                                                                \
																													          \
			template <>                                                                                                       \
			struct ApiFuncTraits<Ludeo##inInterfaceName##CallbackName##CallbackParams>                                        \
				: public ApiFuncTraits<Ludeo##inInterfaceName##inFuncName##Params>                                            \
			{                                                                                                                 \
			};                                                                                                                \
		}
#else
	#define LUDEO_DEFINE_FUNC_TRAITS(...)
	#define LUDEO_DEFINE_FUNC_TRAITS_2(...)
	#define LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK(...)
	#define LUDEO_DEFINE_FUNC_TRAITS_WITHCALLBACK_2(...)
#endif

//
// - An enum value called "_LASTVALUE" is added. This is not to be used by the clients. It's used internally
// - For C, a value _PADDING is added. This is to force the enum to be 32-bits
//

#if __cplusplus
	//
	// If using C++ we use enum classes
	//
	#define LUDEO_ENUM_START(Name) enum class Name : int32_t {
	#define LUDEO_ENUM_END(Name) }
	#define LUDEO_ENUM_VALUE(EnumType, Name, Value) Name = Value,
	#define LUDEO_ENUM_VALUE_LAST(EnumType, Name, Value) \
		Name = Value, \
		_LASTVALUE = Value
#else
	//
	// If using C then:
	// - We use a typedef
	// - We add a last entry to force the compiler to use 32 bits
	#define LUDEO_ENUM_START(Name) typedef enum Name {
	#define LUDEO_ENUM_END(Name) , __##Name##_PADDING = 0x7FFFFFFF } Name

	#define LUDEO_ENUM_VALUE(EnumType, Name, Value) EnumType ## _ ## Name = Value,
	#define LUDEO_ENUM_VALUE_LAST(EnumType, Name, Value) EnumType ## _ ## Name = Value
#endif

//
// Define the Result enum
// If using C++, then a result will be e.g LudeoResult::Success
// If using C, then a result will be LudeoResult_Success
//
#if __cplusplus
	#define LUDEO_LIST_VALUE(Name, Value) Name = Value,
	#define LUDEO_LIST_VALUE_LAST(Name, Value) \
		Name = Value, \
		_LASTVALUE = Value

#else
	#define LUDEO_LIST_VALUE(Name, Value) LudeoResult_ ## Name = Value, 
	#define LUDEO_LIST_VALUE_LAST(Name, Value) LudeoResult_ ## Name = Value
#endif
LUDEO_ENUM_START(LudeoResult)
	#include "Ludeo/EnumResult.h"
LUDEO_ENUM_END(LudeoResult);
#undef LUDEO_LIST_VALUE
#undef LUDEO_LIST_VALUE_LAST

/**
* Boolean type used by the SDK.
* The SDK uses this even for C++ to avoid any ABI problems.
*/
typedef int32_t LudeoBool;
#define LUDEO_TRUE 1
#define LUDEO_FALSE 0

/**
 * Handle to an existing notification the client registered.
 * 0 means it's an invalid handle
 */
EXTERN_C typedef uint64_t LudeoNotificationId;

/**
 * Invalid notification Id. "AddNotifyXXX" functions return this if they failed to register a notification
 */
#define LUDEO_INVALID_NOTIFICATIONID (LudeoNotificationId(0))

