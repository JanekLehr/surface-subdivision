/*
	File:			CLConfig.h

	Function:		Contains configuration options for compiling the CL 
					library.
					
	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott
*/

//
//	Current options are as follows:
//
//	CL_NO_BOOL		- Compiler has no bool type.
//	CL_NO_TF		- true and false are not predefined.
//  CL_TMPL_INST    - use ANSI syntax to explicitly instantiate Array templates
//	CL_SGI_INST		- use sgi's weirdo instantiation syntax.
//

// --- SGI --------------------------------------------------------------------

#ifdef __SGI__
#ifdef _ABIO32
// old CC: doesn't do bool
#define CL_NO_BOOL
#define CL_NO_TF
#endif
#define CL_SGI_INST
#endif

// --- GCC --------------------------------------------------------------------

#ifdef __GCC__
#define CL_TMPL_INST
#endif

// --- Metrowerks, MacOS ------------------------------------------------------

#ifdef __MACOS__
#endif

