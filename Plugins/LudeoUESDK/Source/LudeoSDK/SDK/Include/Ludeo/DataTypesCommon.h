// Copyright 2023 Ludeo. All rights reserved.

#pragma once

#include "Ludeo/Common.h"

#pragma pack(push, 8)

/**
 * Data types the backend understands.
 *
 * Data sent with any of these types can be used to setup the scoring/challenges for a Ludeo
 */
LUDEO_ENUM_START(LudeoDataType)

	/**
	 * Single boolean, as a byte
	 */
	LUDEO_ENUM_VALUE(LudeoDataType, Bool, 0)

	/** Single signed 8-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, Int8, 1)

	/** Single unsigned 8-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, UInt8, 2)

	/** Single signed 16-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, Int16, 3)

	/** Single unsigned 16-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, UInt16, 4)

	/** Single signed 32-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, Int32, 5)

	/** Single unsigned 32-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, UInt32, 6)

	/** Single signed 64-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, Int64, 7)

	/** Single unsigned 64-bits integer */
	LUDEO_ENUM_VALUE(LudeoDataType, UInt64, 8)

	/** Single 32-bits floating point */
	LUDEO_ENUM_VALUE(LudeoDataType, Float, 9)

	/** Single 64-bits floating point */
	LUDEO_ENUM_VALUE(LudeoDataType, Double, 10)

	/**
	 * A null terminated string
	 */
	LUDEO_ENUM_VALUE(LudeoDataType, String, 11)

	/** 
	 * 3 x 32-bits floating point
	 * E.g: A position in space (X,Y,Z)
	 */
	LUDEO_ENUM_VALUE(LudeoDataType, Vec3Float, 12)

	/**
	 * A quaternion (e.g: orientation of a character)
	 *
	 * 4 x 32-bits floating point
	 */
	LUDEO_ENUM_VALUE_LAST(LudeoDataType, Vec4Float, 13)

LUDEO_ENUM_END(LudeoDataType);

/** Type that identifies a single registered Ludeo Object */
typedef uint32_t LudeoObjectId;

/** Value that represents an invalid Ludeo Object */
#define LUDEO_INVALID_OBJECTID 0

#pragma pack(pop)

