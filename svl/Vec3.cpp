/*
	File:			Vec3.cc

	Function:		Implements Vec3.h

	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott

	Notes:			

*/


#include "Vec3.h"
#include <ctype.h>
#include <iomanip>


std::ostream &operator << (std::ostream &s, const TVec3 &v)
{
	Int w = s.width();

	return(s << '[' << v[0] << ' ' << std::setw(w) << v[1] << ' ' << std::setw(w) << v[2] << ']');
}

std::istream &operator >> (std::istream &s, TVec3 &v)
{
    TVec3	result;
    Char	c;
	
	// Expected format: [1 2 3]
	
    while (s >> c && isspace(c))		
		;
		
    if (c == '[')						
    {
		s >> result[0] >> result[1] >> result[2];	

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

