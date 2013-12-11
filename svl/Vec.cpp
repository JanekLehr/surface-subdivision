/*
	File:			Vec.cc

	Function:		Implements Vec.h

	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott

	Notes:			

*/


#include "Vec.h"
#ifndef __SVL__
#include "Array.h"		// for the >> operator...
#endif

#include <iomanip>
#include <string.h>
#include <stdarg.h>


// --- Vec Constructors -------------------------------------------------------


TVec::TVec(Int n, ZeroOrOne k) : elts(n)
{
	Assert(n > 0,"(Vec) illegal vector size");

	data = new TVReal[n];
	Assert(data != 0, "(Vec) Out of memory");	
	
	MakeBlock(k);
}

TVec::TVec(Int n, Axis a) : elts(n)
{
	Assert(n > 0,"(Vec) illegal vector size");

	data = new TVReal[n];
	Assert(data != 0, "(Vec) Out of memory");	
	
	MakeUnit(a);
}

TVec::TVec(const TVec &v)
{
	Assert(v.data != 0, "(Vec) Can't construct from a null vector");
	
	elts = v.Elts();
	data = new TVReal[elts];
	Assert(data != 0, "(Vec) Out of memory");
	// You might wonder why I don't use memcpy here and elsewhere.
	// Timing tests on an R4400 show memcpy only becomes faster for
	// n > 30 or so, and on an R10000, memcpy is never faster. Hence
	// I'm sticking with loop copies for now.
	for (Int i = 0; i < Elts(); i++)
		data[i] = v[i];
}

#ifndef __SVL__
TVec::TVec(const TSubVec &v) : elts(v.Elts())
{
	data = new TVReal[elts];	
	Assert(data != 0, "(Vec) Out of memory");
	
	for (Int i = 0; i < Elts(); i++)
		data[i] = v[i];
}
#endif

TVec::TVec(const TVec2 &v) : elts(v.Elts() | VL_REF_FLAG), data(v.Ref())
{
}

TVec::TVec(const TVec3 &v) : elts(v.Elts() | VL_REF_FLAG), data(v.Ref())
{
}

TVec::TVec(const TVec4 &v) : elts(v.Elts() | VL_REF_FLAG), data(v.Ref())
{
}

TVec::TVec(Int n, double elt0, ...) : elts(n)
{
	Assert(n > 0,"(Vec) illegal vector size");

	va_list ap;
	Int 	i = 1;
		
	data = new TVReal[n];

	va_start(ap, elt0);
		
	SetReal(data[0], elt0);
	
	while (--n)
		SetReal(data[i++], va_arg(ap, double));

	va_end(ap);
}

TVec::~TVec()
{
	if (!IsRef())
		delete[] data;
}


// --- Vec Assignment Operators -----------------------------------------------


TVec &TVec::operator = (const TVec &v)
{
	if (!IsRef())
		SetSize(v.Elts());
	else
		Assert(Elts() == v.Elts(), "(Vec::=) Vector sizes don't match");

	for (Int i = 0; i < Elts(); i++)
		data[i] = v[i];

	return(SELF);
}

#ifndef __SVL__
TVec &TVec::operator = (const TSubVec &v)
{
	if (!IsRef())
		SetSize(v.Elts());
	else
		Assert(Elts() == v.Elts(), "(Vec::=) Vector sizes don't match");

	for (Int i = 0; i < Elts(); i++)
		data[i] = v[i];

	return(SELF);
}
#endif

TVec &TVec::operator = (const TVec2 &v)
{
	if (!IsRef())
		SetSize(v.Elts());
	else
		Assert(Elts() == v.Elts(), "(Vec::=) Vector sizes don't match");
	
	data[0] = v[0];
	data[1] = v[1];
	
	return(SELF);
}

TVec &TVec::operator = (const TVec3 &v)
{
	if (!IsRef())
		SetSize(v.Elts());
	else
		Assert(Elts() == v.Elts(), "(Vec::=) Vector sizes don't match");
	
	data[0] = v[0];
	data[1] = v[1];
	data[2] = v[2];
	
	return(SELF);
}

TVec &TVec::operator = (const TVec4 &v)
{
	if (!IsRef())
		SetSize(v.Elts());
	else
		Assert(Elts() == v.Elts(), "(Vec::=) Vector sizes don't match");
	
	data[0] = v[0];
	data[1] = v[1];
	data[2] = v[2];
	data[3] = v[3];
	
	return(SELF);
}

Void TVec::SetSize(Int n)
{
	if (!IsRef())
	{
		// Don't reallocate if we already have enough storage
	
		if (n <= elts)
		{
			elts = n;
			return;
		}
		
		// Otherwise, delete old storage
		
		delete[] data;
	}
	
	elts = n;
	data = new TVReal[elts];
	Assert(data != 0, "(Vec::SetSize) Out of memory");
}

Void TVec::MakeZero()
{
	Int j;

	for (j = 0; j < Elts(); j++)
		data[j] = vl_zero;	
}

Void TVec::MakeUnit(Int i, TVReal k)
{
	Int j;

	for (j = 0; j < Elts(); j++)
		data[j] = vl_zero;	
		
	data[i] = k;
}

Void TVec::MakeBlock(TVReal k)
{
	Int i;
	
	for (i = 0; i < Elts(); i++)
		data[i] = k;
}


// --- Vec In-Place operators -------------------------------------------------


TVec &operator += (TVec &a, const TVec &b)
{
	Assert(a.Elts() == b.Elts(), "(Vec::+=) vector sizes don't match");	

	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		a[i] += b[i];
	
	return(a);
}

TVec &operator -= (TVec &a, const TVec &b)
{
	Assert(a.Elts() == b.Elts(), "(Vec::-=) vector sizes don't match");	

	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		a[i] -= b[i];
		
	return(a);
}

TVec &operator *= (TVec &a, const TVec &b)
{
	Assert(a.Elts() == b.Elts(), "(Vec::*=) Vec sizes don't match");

	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		a[i] *= b[i];
	
	return(a);
}

TVec &operator *= (TVec &v, TVReal s)
{
	Int		i;
	
	for (i = 0; i < v.Elts(); i++) 
		v[i] *= s;
	
	return(v);
}

TVec &operator /= (TVec &a, const TVec &b)
{
	Assert(a.Elts() == b.Elts(), "(Vec::/=) Vec sizes don't match");

	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		a[i] /= b[i];
	
	return(a);
}

TVec &operator /= (TVec &v, TVReal s)
{
	Int		i;
	
	for (i = 0; i < v.Elts(); i++) 
		v[i] /= s;
	
	return(v);
}


// --- Vec Comparison Operators -----------------------------------------------


Bool operator == (const TVec &a, const TVec &b)
{
	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		if (a[i] != b[i])
			return(0);
	
	return(1);
}

Bool operator != (const TVec &a, const TVec &b)
{
	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		if (a[i] != b[i])
			return(1);
	
	return(0);
}


// --- Vec Arithmetic Operators -----------------------------------------------


TVec operator + (const TVec &a, const TVec &b)
{
	Assert(a.Elts() == b.Elts(), "(Vec::+) Vec sizes don't match");

	TVec	result(a.Elts());
	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		result[i] = a[i] + b[i];
	
	return(result);
}

TVec operator - (const TVec &a, const TVec &b) 
{
	Assert(a.Elts() == b.Elts(), "(Vec::-) Vec sizes don't match");
	
	TVec	result(a.Elts());
	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		result[i] = a[i] - b[i];
	
	return(result);
}

TVec operator - (const TVec &v)
{
	TVec	result(v.Elts());
	Int		i;
	
	for (i = 0; i < v.Elts(); i++) 
		result[i] = - v[i];
	
	return(result);
}

TVec operator * (const TVec &a, const TVec &b)			
{
	Assert(a.Elts() == b.Elts(), "(Vec::*) Vec sizes don't match");
	
	TVec	result(a.Elts());
	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		result[i] = a[i] * b[i];
	
	return(result);
}

TVec operator * (const TVec &v, TVReal s) 
{
	TVec	result(v.Elts());
	Int		i;
	
	for (i = 0; i < v.Elts(); i++) 
		result[i] = v[i] * s;
	
	return(result);
}

TVec operator / (const TVec &a, const TVec &b)			
{
	Assert(a.Elts() == b.Elts(), "(Vec::/) Vec sizes don't match");
	
	TVec	result(a.Elts());
	Int		i;
	
	for (i = 0; i < a.Elts(); i++) 
		result[i] = a[i] / b[i];
	
	return(result);
}

TVec operator / (const TVec &v, TVReal s) 
{
	TVec	result(v.Elts());
	Int		i;
	
	for (i = 0; i < v.Elts(); i++) 
		result[i] = v[i] / s;
	
	return(result);
}

TVReal dot(const TVec &a, const TVec &b) 
{
	Assert(a.Elts() == b.Elts(), "(Vec::dot) Vec sizes don't match");

	TMReal	sum = vl_zero;
	Int		i;
		
	for (i = 0; i < a.Elts(); i++) 
		sum += a[i] * b[i];
	
	return(sum);
}

TVec operator * (TVReal s, const TVec &v)
{
	TVec	result(v.Elts());
	Int		i;
	
	for (i = 0; i < v.Elts(); i++) 
		result[i] = v[i] * s;
	
	return(result);
}


// --- Vec Input & Output -----------------------------------------------------


std::ostream &operator << (std::ostream &s, const TVec &v)
{
	Int i, w;

	s << '[';
	
	if (v.Elts() > 0)
	{
    	w = s.width();
		s << v[0];
	
		for (i = 1; i < v.Elts(); i++)
			s << ' ' << std::setw(w) << v[i];
	}
	
	s << ']';
	
	return(s);
}

#ifndef __SVL__
istream &operator >> (istream &s, TVec &v)
{
	Array<TVReal> array;
	
	// Expected format: [1 2 3 4 ...]

	s >> array;									// Read input into variable-sized array

	v = TVec(array.NumItems(), array.Ref());	// Copy input into vector
	
    return(s);
}
#endif
