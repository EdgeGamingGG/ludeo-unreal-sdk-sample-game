// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"

#pragma pack(push, 8)

/**
 * Log level
 */
LUDEO_ENUM_START(LudeoLogLevel)

	/**
	* Value used to turn off logging
	*/
	LUDEO_ENUM_VALUE(LudeoLogLevel, Off, 0)

	/**
	 * Some fatal error or assert has happened. A log of this level means something critical has failed or an SDK assert has
	 * triggered. This should not occur at all. If it does, it's most likely a bug in the SDK.
	 */
	LUDEO_ENUM_VALUE(LudeoLogLevel, Fatal, 1)

	/**
	 * A non-fatal error has occurred.
	 * Logs of this level normally indicate why an API call has failed, or why some other non-fatal internal code has
	 * failed.
	 */
	LUDEO_ENUM_VALUE(LudeoLogLevel, Error, 2)

	/** Warning */
	LUDEO_ENUM_VALUE(LudeoLogLevel, Warning, 3)

	/** Normal log level */
	LUDEO_ENUM_VALUE(LudeoLogLevel, Log, 4)

	/** */
	LUDEO_ENUM_VALUE(LudeoLogLevel, Verbose, 5)

	/** */
	LUDEO_ENUM_VALUE_LAST(LudeoLogLevel, VeryVerbose, 6)
LUDEO_ENUM_END(LudeoLogLevel);

/**
 * Log category
 */
LUDEO_ENUM_START(LudeoLogCategory)
	/**
	 * Log category used for any logging not related to a specific interface or service
	 */
	LUDEO_ENUM_VALUE(LudeoLogCategory, Core, 0)

	/**
	 * Log category used for any logging related to the session interface
	 */
	LUDEO_ENUM_VALUE(LudeoLogCategory, Session, 1)

	/**
	 * Log category for Http communications
	 */
	LUDEO_ENUM_VALUE(LudeoLogCategory, Http, 2)

	/**
	 * Log category for writing and reading gameplaydata
	 */
	LUDEO_ENUM_VALUE(LudeoLogCategory, Data, 3)

	/**
	 * Log category for any logging related to rooms 
	 */
	LUDEO_ENUM_VALUE(LudeoLogCategory, Room, 4)

	/**
	 * Log category for any logging related to the overlay
	 */
	LUDEO_ENUM_VALUE(LudeoLogCategory, Overlay, 5)

	/**
	 * Log category for the logging coming from Coherent's gameface
	 */
	LUDEO_ENUM_VALUE(LudeoLogCategory, Coherent, 6)

	/** Not a real category. This is a special value that can be passed to ludeo_setLoggingLevel */
	LUDEO_ENUM_VALUE_LAST(LudeoLogCategory, All, 0x7FFFFFFF)
LUDEO_ENUM_END(LudeoLogCategory);

/** Signature for the memory allocation function */
EXTERN_C typedef void* (LUDEO_CALL * LudeoMemoryAllocFunc)(size_t size);
/** Signature for the memory reallocation function */
EXTERN_C typedef void* (LUDEO_CALL * LudeoMemoryReallocFunc)(void* ptr, size_t size);
/** Signature for the memory free function */
EXTERN_C typedef void  (LUDEO_CALL * LudeoMemoryFreeFunc)(void* ptr);

/** The most recent version of the ludeo_Initialize API */
#define LUDEO_INITIALIZE_API_LATEST 1


// #RVF : Need to consider if we should really expose all this. My reasoning here is that different games will have different
// needs, but by exposing all this, it makes it harder to later remove it.

/**
 * The SDK uses Remotery (https://github.com/Celtoys/Remotery) for profiling.
 * These parameters directly map to fields in Remotery's rmtSettings struct.
 * Any fields the client doesn't need to change should be set to the recommended values
 */

LUDEO_STRUCT(LudeoProfilerSettings, (

	/**
	 * Maps to rmtSettings::port
	 *
	 * Which port to listen for incoming connections on
	 * Recommended port is 17815
	 */
	uint16_t port;

	/**
	 * Maps to rmtSettings::reuse_open_port
	 *
	 * When this server exits it can leave the port open in TIME_WAIT state for a while. This forces
	 * subsequent server bind attempts to fail when restarting. If you find restarts fail repeatedly
	 * with bind attempts, set this to true to forcibly reuse the open port.
	 */
	LudeoBool reuseOpenPort;

	/**
	 * Maps to rmtSettings::limit_connections_to_localhost
	 *
	 * Only allow connections on localhost?
	 * For dev builds you may want to access your game from other devices but if
	 * you distribute a game to your players with Remotery active, probably best
	 * to limit connections to localhost.
	 */
	LudeoBool limitConnectionsToLocalhost;

	/*
	 * Maps to rmtSettings::enableThreadSampler
	 * 
	 * Whether to enable runtime thread sampling that discovers which processors a thread is running
	 * on. This will suspend and resume threads from outside repeatdly and inject code into each
	 * thread that automatically instruments the processor.
	 *
	 * Recommended value is True
	 */
	 LudeoBool enableThreadSampler;

	/*
	 * Maps to rmtSettings::msSleepBetweenServerUpdates
	 *
	 * How long to sleep between server updates, hopefully trying to give
	 * a little CPU back to other threads.
	 *
	 * Recommended value is 4 (milliseconds)
	 */
	uint32_t msSleepBetweenServerUpdates;

	/*
	 * Maps to rmtSettings::messageQueueSizeInBytes
	 * 
	 * Size of the internal message queues Remotery uses
	 * Will be rounded to page granularity of 64k
	 *
	 * Recommended value is 1024*1024
	 */
	uint32_t messageQueueSizeInBytes;

	/*
	 * Maps to rmtSettings::maxNbMessagesPerUpdate
	 *
	 * If the user continuously pushes to the message queue, the server network
	 * code won't get a chance to update unless there's an upper-limit on how
	 * many messages can be consumed per loop.
	 *
	 * Recommended value is 1000
	 */
	uint32_t maxNbMessagesPerUpdate;
));

/**
 * Parameters passed to ludeo_Initialize
 */
LUDEO_STRUCT(LudeoInitializeParams, (
	/** Set this to LUDEO_INITIALIZE_API_LATEST */
	int32_t apiVersion;

	/**
	 * Memory allocation function. If nulled, it will use the default implementation.
	 * If specified, all other memory functions need to be specified.
	 */
	LudeoMemoryAllocFunc memoryAllocFunc;

	/**
	 * Memory reallocation function. If nulled, it will use the default implementation
	 * If specified, all other memory functions need to be specified.
	 */
	LudeoMemoryReallocFunc memoryReallocFunc;

	/**
	 * Memory release function. If nulled, it will use the default implementation
	 * If specified, all other memory functions need to be specified.
	 */
	LudeoMemoryFreeFunc memoryFreeFunc;

	 /**
	  * Pointer to the profiler settings.
	  * Can be nulled, in which case profiling is disabled.
	  *
	  * Note that profiling is non-existent in the Release version of the SDK. Only the Development build contains profiling code.
	  * This field is ignored in any builds that doesn't support profiling.
	  */
	 LudeoProfilerSettings* profilerSettings;

));

/** The most recent version of ludeo_SetLoggingToFile */
#define LUDEO_SETLOGGINGTOFILE_API_LATEST 1

/**
 * Structure for parameters passed to ludeo_SetLoggingToFile
 */
LUDEO_STRUCT(LudeoSetLoggingToFileParams, (
	/** Set this to LUDEO_SETLOGGINGTOFILE_API_LATEST */
	int32_t apiVersion;

	/**
	 * If set to true, then file logging is enabled
	 * If set to false, it disables file logging (if previously enabled)
	 */
	LudeoBool enable; 

	/**
	 * If enable is true, then this specifies in what directory should the log file be created.
	 * If nulled, then the current working directory will be used
	 */
	const char* directory;
));

/**
 * Structure that represents a single log message
 * Any pointers in the struct will be temporary or internal to the SDK, and should therefore NOT be kept by the client
 */
LUDEO_STRUCT(LudeoLogMessage, (
	/**
	 * Log level for this message
	 * Use ludeo_LogLevelToString/to_string to get the string equivalent
	 */
	LudeoLogLevel level;

	/**
	 * The log category
	 * Use ludeo_LogCategoryToString/to_string to get the string equivalent
	 */
	LudeoLogCategory category;

	/** String with UTC time. HH:mm: */
	const char* timestamp;

	/**
	 * UTF-8 log message
	 * This is intentionally just the log message without any prefixes, so the client can format the final output as desired.
	 * Recommended format is (in printf style):
	 *     printf("s%:%s:%s: %s", msg->timestamp, msg->categoryStr, msg->levelStr, msg->message)
	 *
	 * Note that the message doesn't include a \n . It's up to the client to add that if required.
	 */
	const char* message;
));

/** Signature for the logging function */
EXTERN_C typedef void (LUDEO_CALL * LudeoLogCallback)(const LudeoLogMessage* data);

#pragma pack(pop)

