
#include "CoreTracy.h"

#if CE_ENABLE_TRACY
#	define TRACY_ENABLE
#else
#	undef  TRACY_ENABLE
#endif

#include "TracyClient.cpp"

