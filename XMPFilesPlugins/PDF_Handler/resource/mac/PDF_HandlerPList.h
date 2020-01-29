#include "../../../../build/XMP_BuildInfo.h"
#define PDF_HANDLER_VERSION 1.0
#if NDEBUG	// Can't use XMP_Environment.h, it seems to mess up the PList compiler.
	#define kConfig Release
	#define kDebugSuffix
	#define kBasicVersion PDF_HANDLER_VERSION
#else
	#define kConfig Debug
	#define kDebugSuffix (debug)
	#define kBasicVersion PDF_HANDLER_VERSION kDebugSuffix
#endif

