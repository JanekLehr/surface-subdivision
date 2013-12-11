/*
	File:		VLConstants.h
	
	Purpose:	Contains various constants for VL.
	
	Author:		Andrew Willmott.
*/

#ifndef __VL_CONSTANTS__
#define __VL_CONSTANTS__


//    --- Mathematical constants ----------------------------------------------

// God knows why this isn't defined in math.h in the magical world of MSVC.

#define M_PI            3.14159265358979323846
#define M_PI_2          1.57079632679489661923

#ifdef M_PI
const Real			vl_pi = M_PI;
const Real			vl_halfPi = M_PI_2;
#else
const Real			vl_pi = _PI;
const Real			vl_halfPi = _PI / 2.0;
#endif

enum	ZeroOrOne	{ vl_zero = 0, vl_0 = 0, vl_one = 1, vl_I = 1, vl_1 = 1 };
enum	Block		{ vl_Z = 0, vl_B = 1 };
enum	Axis		{ vl_x, vl_y, vl_z, vl_w };
typedef	Axis		vl_axis;

const UInt32		VL_REF_FLAG = UInt32(1) << (sizeof(UInt32) * 8 - 1);
const UInt32 		VL_REF_MASK = (~VL_REF_FLAG);
enum				{ VL_SV_END = -1 };
const UInt32		VL_SV_MAX_INDEX = (1 << 30);
const Int 			VL_MAX_STEPS = 10000;

#endif
