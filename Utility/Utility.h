// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the UTILITY_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// UTILITY_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef UTILITY_EXPORTS
#define UTILITY_API __declspec(dllexport)
#else
#define UTILITY_API __declspec(dllimport)
#endif

// This class is exported from the Utility.dll
class UTILITY_API CUtility {
public:
	CUtility(void);
	// TODO: add your methods here.
};

extern UTILITY_API int nUtility;

UTILITY_API int fnUtility(void);
