// Copyright 2023 Ludeo. All rights reserved.

// Do not include this file directly.

/** Success */
LUDEO_LIST_VALUE(Success, 0)

/** An invalid apiVersion was passed to a function call */
LUDEO_LIST_VALUE(InvalidVersion, 1)

/** An invalid parameter was passed to a function */
LUDEO_LIST_VALUE(InvalidParameters, 2)

/**
 * Invalid authentication details 
 * E.g: The wrong API key was specified, or authentication failed (either the one explicitly specified or automatic auth)
 */
LUDEO_LIST_VALUE(InvalidAuth, 3)

/**
 * A resource was not found. 
 * E.g: The client passed an invalid ludeo Id to ludeo_Session_GetLudeo
 */
LUDEO_LIST_VALUE(NotFound, 4)

/**
 * Operation timed out.
 * E.g, a network operation timed out.
 */
LUDEO_LIST_VALUE(TimedOut, 5)

/** 
 * An error that we cannot identify has occurred.
 * Check the logs for more info.
 */
LUDEO_LIST_VALUE(Unknown, 6)

/**
 * The operation failed because the relevant instance was not in a state valid to perform the request operation.
 * This can be for example trying to initialize a system which is already initialized, shutting down when it's not initialized,
 * or the SDK is not initialized.
 */
LUDEO_LIST_VALUE(WrongState, 7)

/**
 *
 * The SDK was disabled by the backend, or the specific feature the client is trying to use is not permitted due to user consent
 * restrictions.
 * 
 */
LUDEO_LIST_VALUE(SDKDisabled, 8)

/**
 * An operation failed because something went wrong establishing a network connection
 */
LUDEO_LIST_VALUE(NetworkError, 9)

/**
 * An operation was canceled.
 * One example of when this can happen is when pending operation are canceled because the SDK is shutting down
 */
LUDEO_LIST_VALUE(Canceled, 10)

/**
 * Indicates the SDK received invalid configuration from the backend.
 */
LUDEO_LIST_VALUE(InvalidConfiguration, 11)

/**
 * Something the client requested or provided is of the wrong type.
 */
LUDEO_LIST_VALUE(WrongType, 12)

/**
 * The call failed because the SDK encountered invalid data.
 * A situation like this can happen if the SDK receives ludeo data from the backend that is invalid somehow. E.g, problems related
 * to the data and object types the client previously registered.
 */
LUDEO_LIST_VALUE_LAST(InvalidData, 13)

