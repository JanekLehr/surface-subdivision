/*
	File:			Basics.h

	Function:		Basic definitions for all files. Contains type definitions,
					assertion and debugging facilities, and miscellaneous
					useful template functions.
					
	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott
	
	Notes:			This header is affected by the following defines:

					CL_CHECKING		- Include code for assertions,
									  range errors and warnings.
					CL_DEBUG		- Enables misc. debugging statements.
					CL_FLOAT		- Use floats for real numbers. (Doubles
									  are the default.)
					CL_NO_BOOL		- There is no bool type.
					CL_NO_TF		- true and false are not predefined.
*/

#ifndef __Basics__
#define __Basics__  

#include "CLConfig.h"
#include <iostream>
#include <ostream>
// #include <math.h>
#include <cmath>

// using namespace std;


// --- Basic types -------------------------------------------------------------

typedef void			Void;
typedef float			Float;
typedef double			Double;

typedef int				Int;
typedef long			Long;

typedef signed int		SInt;
typedef signed long		SLong;
typedef signed short	SInt16;
typedef signed long		SInt32;

typedef unsigned int	UInt;
typedef unsigned long	ULong;
typedef unsigned short	UInt16;
typedef unsigned long	UInt32;

typedef unsigned long	Addr;
// Integral type that is the same size as a pointer.

typedef unsigned char	Byte;
typedef unsigned char	SByte;
typedef char			Char;

#ifndef CL_FLOAT
typedef Double			Real;
#else
typedef Float			Real;
#endif

#define SELF (*this)	// A syntactic convenience.	


// --- Boolean type ------------------------------------------------------------

// X11 #defines 'Bool' -- typical.

#ifdef Bool	
#undef Bool
#endif

#ifndef CL_NO_BOOL
// if the compiler implements the bool type...
typedef bool Bool;
#else
// if not, make up our own.
class Bool 
{
public:
	
	Bool() : val(0) {};
	Bool(Int b) : val(b) {};

	operator Int() { return val; };
	
private:
	Int val;
};
#ifdef CL_NO_TF
enum {false, true};
#endif
#endif

std::ostream &operator << (std::ostream &s, Bool &b);


// --- Assertions and Range checking -------------------------------------------

#define _Error(e)				_Assert(false, e, __FILE__, __LINE__)
#define _Warning(w)				_Expect(false, w, __FILE__, __LINE__)

#if defined(CL_CHECKING) || defined(VL_CHECKING)
#define Assert(b, e)			_Assert(b, e, __FILE__, __LINE__)
	// Assert that b is true. e is an error message to be printed if b
	// is false.
#define Expect(b, w)			_Expect(b, w, __FILE__, __LINE__)
	// Prints warning w if b is false
#define CheckRange(i, l, u, r)	_CheckRange(i, l, u, r, __FILE__, __LINE__)
	// Checks whether i is in the range [lowerBound, upperBound).
#else
#define Assert(b, e)
#define Expect(b, w)
#define CheckRange(a, l, u, r)
#endif

Void _Assert(Int condition, Char *errorMessage, Char *file, Int line);
Void _Expect(Int condition, Char *warningMessage, Char *file, Int line);
Void _CheckRange(Int i, Int lowerBound, Int upperBound, char *rangeMessage,
		Char *file, Int line);


// --- Inlines -----------------------------------------------------------------

template<class Value>
	inline Value Min(Value x, Value y)
	{
		if (x <= y)
			return(x);
		else
			return(y);
	};
	
template<class Value>
	inline Value Max(Value x, Value y)
	{
		if (x >= y)
			return(x);
		else
			return(y);
	};
	
template<class Value>
	inline Void Swap(Value &x, Value &y)
	{
		Value t;
		
		t = x;
		x = y;
		y = t;
	};
	
template<class Value>
	inline Value Mix(Value x, Value y, Real s)
	{
		return(x + (y - x) * s);
	};

template<class Value>
	inline Value Clip(Value x, Value min, Value max)
	{
		if (x < min)
			return(min);
		else if (x > max)
			return(max);
		else
			return(x);
	};

template<class Value>
	inline Value sqr(Value x)
	{
		return(x * x);
	};

#endif
