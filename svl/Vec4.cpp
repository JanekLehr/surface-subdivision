/*
	File:			Vec4.cc

	Function:		Implements Vec4.h

	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott

	Notes:			

*/


#include "Vec4.h"
#include <ctype.h>
#include <iomanip>


Void TVec4::MakeUnit(Int n, TVReal k)
{
	if (n == 0)
	{ elt[0] = k; elt[1] = vl_zero; elt[2] = vl_zero; elt[3] = vl_zero; }
	else if (n == 1)
	{ elt[0] = vl_zero; elt[1] = k; elt[2] = vl_zero; elt[3] = vl_zero; }
	else if (n == 2)
	{ elt[0] = vl_zero; elt[1] = vl_zero; elt[2] = k; elt[3] = vl_zero; }
	else if (n == 3)
	{ elt[0] = vl_zero; elt[1] = vl_zero; elt[2] = vl_zero; elt[3] = k; }
	else 
		_Error("(Vec4::MakeUnit) illegal unit vector");
}

Bool TVec4::operator == (const TVec4 &a) const
{
	return(elt[0] == a[0] && elt[1] == a[1] && elt[2] == a[2] && elt[3] == a[3]);
}

Bool TVec4::operator != (const TVec4 &a) const
{
	return(elt[0] != a[0] || elt[1] != a[1] || elt[2] != a[2] || elt[3] != a[3]);
}

TVec4 cross(const TVec4 &a, const TVec4 &b, const TVec4 &c)	
{
	TVec4 result;
	// XXX can this be improved? Look at assembly.
#define ROW(i)		 a[i], b[i], c[i]
#define DET(i,j,k)   dot(TVec3(ROW(i)), cross(TVec3(ROW(j)), TVec3(ROW(k))))
	
	result[0] =  DET(1,2,3);
	result[1] = -DET(0,2,3);
	result[2] =  DET(0,1,3);
	result[3] = -DET(0,1,2);
	
	return(result);

#undef ROW
#undef DET
}

TVec3 proj(const TVec4 &v)		 
{
	TVec3 result;
	
	Assert(v[3] != 0, "(Vec4/proj) last elt. is zero");
	
	result[0] = v[0] / v[3];
	result[1] = v[1] / v[3];
	result[2] = v[2] / v[3];
	
	return(result);
}


std::ostream &operator << (std::ostream &s, const TVec4 &v)
{
	Int w = s.width();
		
	return(s << '[' << v[0] << ' ' << std::setw(w) << v[1] << ' '
		<< std::setw(w) << v[2] << ' ' << std::setw(w) << v[3] << ']');
}

std::istream &operator >> (std::istream &s, TVec4 &v)
{
    TVec4	result;
    Char	c;
	
	// Expected format: [1 2 3 4]
	
    while (s >> c && isspace(c))		
		;
		
    if (c == '[')						
    {
		s >> result[0] >> result[1] >> result[2] >> result[3];	

		if (!s)
		{
			std::cerr << "Error: Expected number while reading vector\n";
			return(s);
		}
			
		while (s >> c && isspace(c))
			;
			
		if (c != ']')
    	{
			s.clear(std::ios::failbit);
			std::cerr << "Error: Expected ']' while reading vector\n";
	    	return(s);
    	}
	}
    else
	{
		s.clear(std::ios::failbit);
		std::cerr << "Error: Expected '[' while reading vector\n";
	    return(s);
	}
	
	v = result;
    return(s);
}

