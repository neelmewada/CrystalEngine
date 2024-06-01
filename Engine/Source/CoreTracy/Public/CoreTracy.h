#pragma once

#if CE_ENABLE_TRACY
#	define TRACY_ENABLE
#else
#	undef  TRACY_ENABLE
#endif

#include "tracy/Tracy.hpp"

