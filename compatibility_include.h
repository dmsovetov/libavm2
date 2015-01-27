// Dummy header; should get included first into tu-testbed headers.
// This is for manual project-specific configuration.

//
// Some optional general configuration.
//


#define TU_CONFIG_LINK_TO_THREAD 0
//#define TU_USE_OGLES
//#define TU_USE_OPENAL

#ifdef _WINDOWS
	#define strcasecmp _stricmp 
#endif

