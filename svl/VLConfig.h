/*
	File:			VLConfig.h

	Function:		Contains configuration options for compiling the VL 
					library.
					
	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott
*/

//
//	Current options are as follows:
//
//	VL_HAS_ABSF		- has the absf() call. 
//	VL_ROW_ORIENT	- Use row-oriented transforms, so you can swap 'em with 
//					  OpenGL. If this is defined, transformations are 
//                    v = v * Rot3x(u, 0.5), rather than v = Rot3x(u, 0.5) * v.
//                    This is off by default.
//

// --- SGI --------------------------------------------------------------------

#ifdef __SGI__
#define VL_HAS_ABSF
#endif


