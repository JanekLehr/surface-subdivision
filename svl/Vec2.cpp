/*
	File:			Vec2.cc

	Function:		Implements Vec2.h

	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott

	Notes:			

*/


#include "Vec2.h"
#include <ctype.h>
#include <iomanip>


std::ostream &operator << (std::ostream &s, const TVec2 &v)
{
	Int w = s.width();

	return(s << '[' << v[0] << ' ' << std::setw(w) << v[1] << ']');
}

std::istream &operator >> (std::istream &s, TVec2 &v)
{
    TVec2	result;
    Char	c;
	
	// Expected format: [1 2]
	
    while (s >> c && isspace(c))		
		;
		
    if (c == '[')						
    {
		s >> result[0] >> result[1];	

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

