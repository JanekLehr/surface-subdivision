/*
	File:			Vec.h

	Function:		Defines a generic resizeable vector.
					
	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott
 */

#ifndef __Vec__
#define __Vec__

#include "VL.h"
// Defines the actual type for TVec etc.

#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"
#define __SVL__
#ifndef __SVL__
#include "SubVec.h"
#endif
#include <iostream>


// --- Vec Class --------------------------------------------------------------


class TVec
{
public:

	// Constructors
	
	inline TVec();						// Null vector: space allocated later
	inline TVec(Int n);										
	TVec(Int n, double elt0, ...);		// Vec(3, 1.1, 2.0, 3.4)
	inline TVec(Int n, TVReal *data);	// Vector pointer...
	TVec(const TVec &v);			 	// Copy constructor
#ifndef __SVL__
	TVec(const TSubVec &v);			 	
#endif
	TVec(const TVec2 &v);			 	
	TVec(const TVec3 &v);			 	
	TVec(const TVec4 &v);			 	
	TVec(Int n, ZeroOrOne);				// Zero or all-ones vector	
	TVec(Int n, Axis a);				// Unit vector
	
   ~TVec();			 					// Destructor
	
	// Accessor functions
		
	inline Int		Elts() const;

	inline TVReal    &operator [] (Int i);	
	inline TVReal 	operator [] (Int i) const;			  

	inline TVReal	*Ref() const;					// Return pointer to data
	
	// Assignment operators
	
	TVec	 		&operator =  (const TVec &v);	// v = a etc.
#ifndef __SVL__
	TVec			&operator =  (const TSubVec &v);		
#endif
	TVec			&operator =  (ZeroOrOne k);
	TVec			&operator =  (Axis a);
	TVec 			&operator =  (const TVec2 &v);			
	TVec	 		&operator =  (const TVec3 &v);			
	TVec			&operator =  (const TVec4 &v);				
		
	Void			SetSize(Int n);					// resize the vector
	
	//	Vector initialisers
	
	Void			MakeZero();
	Void			MakeUnit(Int i, TVReal k = vl_one);
	Void			MakeBlock(TVReal k = vl_one);
	
	Bool			IsRef() const { return(elts & VL_REF_FLAG); };

protected:
	TVReal			*data; 
	UInt32			elts;

};


// --- Vec In-Place operators -------------------------------------------------

TVec		&operator += (TVec &a, const TVec &b);
TVec		&operator -= (TVec &a, const TVec &b);
TVec		&operator *= (TVec &a, const TVec &b);
TVec		&operator *= (TVec &v, TVReal s);
TVec		&operator /= (TVec &a, const TVec &b);
TVec		&operator /= (TVec &v, TVReal s);

// --- Vec Comparison Operators -----------------------------------------------

Bool		operator == (const TVec &a, const TVec &b);
Bool		operator != (const TVec &a, const TVec &b);

// --- Vec Arithmetic Operators -----------------------------------------------

TVec		operator + (const TVec &a, const TVec &b);
TVec		operator - (const TVec &a, const TVec &b);
TVec		operator - (const TVec &v);
TVec		operator * (const TVec &a, const TVec &b);		
TVec		operator * (const TVec &v, TVReal s);
TVec		operator / (const TVec &a, const TVec &b);
TVec		operator / (const TVec &v, TVReal s);
TVec		operator * (TVReal s, const TVec &v);

TVReal			dot(const TVec &a, const TVec &b);	// v . a
inline TVReal	len(const TVec &v);					// || v ||
inline TVReal	sqrlen(const TVec &v);				// v . v
inline TVec		norm(const TVec &v);				// v / || v ||
inline Void		normalise(TVec &v);					// v = norm(v)

// --- Vec Input & Output -----------------------------------------------------

std::ostream		&operator << (std::ostream &s, const TVec &v);
std::istream		&operator >> (std::istream &s, TVec &v);

// --- Sub-vector functions ---------------------------------------------------

inline TVec	sub(const TVec &v, Int start, Int length); 	
inline TVec	first(const TVec &v, Int length); 	
inline TVec	last(const TVec &v, Int length); 	


// --- Vec inlines ------------------------------------------------------------


inline TVec::TVec() : elts(0), data(0)
{
}

inline TVec::TVec(Int n) : elts(n)
{
	Assert(n > 0,"(Vec) illegal vector size");

	data = new TVReal[n];
	Assert(data != 0, "(Vec) Out of memory");	
}

inline TVec::TVec(Int n, TVReal *data) : elts(n | VL_REF_FLAG), data(data)
{
}

inline Int TVec::Elts() const
{
	return(elts & VL_REF_MASK);
}

inline TVReal &TVec::operator [] (Int i)
{
	CheckRange(i, 0, Elts(), "Vec::[i]");
	
    return(data[i]);
}

inline TVReal TVec::operator [] (Int i) const
{
	CheckRange(i, 0, Elts(), "Vec::[i]");

    return(data[i]);
}

inline TVReal *TVec::Ref() const
{
	return(data);
}

inline TVec &TVec::operator = (ZeroOrOne k)
{
	MakeBlock(k);

	return(SELF);
}

inline TVec &TVec::operator = (Axis a)
{
	MakeUnit(a);

	return(SELF);
}

inline TVReal len(const TVec &v)
{
	return(sqrt(dot(v, v)));
}

inline TVReal sqrlen(const TVec &v)
{
	return(dot(v, v));
}

inline TVec norm(const TVec &v)	
{
	return(v / len(v));
}

inline Void normalise(TVec &v)
{
	v /= len(v);
}

inline TVec sub(const TVec &v, Int start, Int length)
{
	Assert(start >= 0 && length > 0 && start + length <= v.Elts(),
		"(sub(Vec)) illegal subset of vector");

	return(TVec(length, v.Ref() + start));
}

inline TVec first(const TVec &v, Int length)
{
	Assert(length > 0 && length <= v.Elts(),
		"(first(Vec)) illegal subset of vector");

	return(TVec(length, v.Ref()));
}

inline TVec last(const TVec &v, Int length)
{
	Assert(length > 0 && length <= v.Elts(),
		"(last(Vec)) illegal subset of vector");

	return(TVec(length, v.Ref() + v.Elts() - length));
}

#endif

