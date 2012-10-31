// Utility.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Utility.h"


// This is an example of an exported variable
UTILITY_API int nUtility=0;

// This is an example of an exported function.
UTILITY_API int fnUtility(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see Utility.h for the class definition
CUtility::CUtility()
{
	return;
}
