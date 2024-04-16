# Ludeo Core SDK Overview

# String format

The API uses ansi and UTF8 (where required).

# Threading model

* Access to the SDK should be done from a single thread (the thread that calls `ludeo_Tick`)
* Callbacks are only ever called from inside a call to `ludeo_Tick` or `ludeo_Shutdown`
	* This allows the client to control when it receives callbacks.
* With very few exceptions, callback are never called in-place. They are always queued to be process during a tick.
	* For example, if a call to an async function fails because of bad parameters, the callback will be deferred even though that function knows straight away that it will fail. This provides a consistent behaviour. If you do see a case where a callback is called in-place, consider it an SDK bug and do let us know.
	* Exceptions to this rule are the logging callback (see `ludeo_SetLoggingCallback`) and memory hooks (see `ludeo_Initialize`)

# Memory hooks

Although not required, memory hooks can be specified with the `ludeo_Initialize` functions. The SDK will use the provided functions to allocate and free memory.
This feature is currently being worked on, and should not be used in production.
This should be mostly useful on Windows, where hooks should be able to catch most of the memory allocations.
On Linux, for technical reasons (due to shared libraries), it's not as accurate as on Windows.

# Overview of the API

* `ludeo_Initialize` should be the first function called, and `ludeo_Shutdown` should be the last. `ludeo_ResultToString` is the only function that can be called after `ludeo_Shutdown`.
* Asynchronous functions don't have a return value (their result will be in the callback). Since `ludeo_Initialize` needs to be called first to initialize the internal systems that deal with callbacks, they fail silently in the following situations:
	* `ludeo_Initialize` wasn't called first, so it fails silently because the callback mechanisms aren't running to report the error.
	* The `callback` parameter is null, so there is no way to report the error


A typical flow for a game would be:

1. Call `ludeo_Initialize` - This initializes the SDK's root systems
2. **OPTIONAL**: Set up any logging with the `ludeo_SetLoggingXXX` functions
3. Call `ludeo_Session_Create` . There is no need to create/destroy several sessions. One for the entire duration of the game is enough
4. Call any of the `ludeo_Session_XXX` functions during gameplay, to either create or play ludeos
5. Call `ludeo_Session_Release`
6. Call `ludeo_Shutdown`

# Callbacks

The SDK provides two types of callbacks.

* Completion callbacks - These are the callbacks that are a reply to a specific async SDK call. E.g: `ludeo_Session_Create`. They match 1-to-1 with the call the client made.
* Notification callbacks - These are callbacks that can happen spontaneously (e.g, when a new ludeo is selected externally to the client). The client needs to explicitly register to receive these. E.g, with the `ludeo_Session_AddNotifyLudeoSelected`.
	* `ludeo_RemoveNotification` can be used to unregister. 

# Utility code

## Converting enums to strings

The file `Utils.h` provides some utility code, such as converting an enum value to a string (e.g: For error reporting).
The C functions to convert an enum to a string are declared as `ludeo_<ENUM NAME>_ToString` (e.g: `ludeo_Result_ToString`).
In addition, if using the SDK from C++, the client can define `LUDEOSDK_TO_STRING` to `1` and `LUDEOSDK_TO_STRING_TYPE` to a typename before including `Utils.h`.
That will declare/define generic `to_string` and `to_cstring` functions for every enum.
E.g, if only including `Utils.h` from one place:

```
#define LUDEOSDK_TO_STRING 1

// If anything other than std::string is desired, then set it with this define.
// If not defined, to_string will use std::string as the return type
#define LUDEOSDK_TO_STRING_TYPE MyString
```

* The declared `to_string` functions return `std::string` or the type defined with `LUDEOSDK_TO_STRING_TYPE`
* The declared `to_cstring` return `const char*`


## Function traits

**NOTE**: This is work in progress and might change in the future.

For most of the SDK functions, if using the SDK from C++, function traits templates are declared that might be useful in generic code.
This is not required to use the SDK.

The traits struct is as follows:

```cpp
namespace Ludeo
{
	template<typename FunctionParameters>
	struct ApiFuncTraits<SomeFunctionParams>
	{
		// The value to use for the `apiVersion` field of the params struct
		static constexpr int32_t latestApiVersion = LUDEO_<function name>_API_LATEST;

		// The type of params struct passed to the SDK function
		using ParamsType = SomeFunctionParams;

		// Returns the function name
		static constexpr const char* funcName() { return "ludeo_<function name>"; }

		// The SDK function signature
		using FuncType = decltype(&ludeo_<function name>);

		// Returns the SDK function
		static FuncType func()
		{
			return ludeo_<function name>;
		}

		//
		// If the function is async or has a callback parameter for any other reason, it declares these:

		// The function callback type
		using CallbackType = Ludeo<function name>Callback;
		// Callback type in the form of std::function<xxx>
		using CallbackTypeCpp = std::function< ... >;
		// Callback parameters type
		using CallbackparamsType = ...;
	};
}
```

For example the traits for the `ludeo_Session_Create` traits are:

```cpp
namespace Ludeo
{
	template <>
	struct ApiFuncTraits<LudeoSessionCreateParams>
	{
		static constexpr int32_t latestApiVersion = LUDEO_SESSION_CREATE_API_LATEST;
		using ParamsType = LudeoSessionCreateParams;
		static constexpr const char* funcName()
		{
			return "ludeo_Session_Create";
		}
		using FuncType = decltype(&ludeo_Session_Create);
		static FuncType func()
		{
			return &ludeo_Session_Create;
		}
		using CallbackType = LudeoSessionCreateCallback;
		using CallbackTypeCpp = std::function<void(const LudeoSessionCreateCallbackParams* data)>;
		using CallbackParamsType = LudeoSessionCreateCallbackParams;
	};

	template <>
	struct ApiFuncTraits<LudeoSessionCreateCallbackParams> : public ApiFuncTraits<LudeoSessionCreateParams>
	{
	};
}	// namespace Ludeo
```

These traits allow generic code such as the `Ludeo::create<>()` utility function.
For example, to initialize a `LudeoSessionCreateParams` :


```cpp

// The C way
LudeoSessionCreateParams params{};
// Explicitly setting apiVersion is error prone.
params.apiVersion = LUDEO_SESSION_CREATE_API_LATEST;

// The C++ way
// Automatically zeroes the struct and sets apiVersion;
auto params = Ludeo::create<LudeoSessionCreateParams>();
```

