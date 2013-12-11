/*
	File:			Basics.cc

	Function:		Implements Basics.h

	Author(s):		Andrew Willmott

	Copyright:		Copyright (c) 1995-1996, Andrew Willmott

	Notes:			

*/

#include "Basics.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>


// --- Error functions for range and routine checking -------------------------


std::ostream &operator << (std::ostream &s, Bool &b)
{
	if (b)
		s << "true";
	else
		s << "false";

	return(s);
}

Void _Assert(Int condition, char *errorMessage, Char *file, Int line)
{
	if (!condition)
	{
		char reply;
		
		std::cerr << "\n*** Assert failed (line " << line << " in " <<
			file << "): " << errorMessage << std::endl;
		std::cerr << "    Continue? [y/n] ";
		std::cin >> reply;
		
		if (reply != 'y')
		{
			*((long *) 0) = 0; // Force a core dump/debugger break
			exit(1);
		}
	}
}

Void _Expect(Int condition, char *warningMessage, Char *file, Int line)
{
	if (!condition)
		std::cerr << "\n*** Warning (line " << line << " in " << file << "): " <<
			warningMessage << std::endl;
}

Void _CheckRange(Int i, Int lowerBound, Int upperBound, 
				 Char *rangeMessage, Char *file, Int line)
{
	if (i < lowerBound || i >= upperBound)
	{
		char reply;
		
		std::cerr << "\n*** Range Error (line " << line << " in " << file <<
			"): " << rangeMessage << std::endl;	
		std::cerr << "    Continue? [y/n] ";
		std::cin >> reply;
		
		if (reply != 'y')
		{
			*((long *) 0) = 0;
			exit(1);
		}
	}
}

