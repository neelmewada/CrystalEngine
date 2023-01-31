#pragma once


#if PAL_TRAIT_BUILD_EDITOR // Editor
#include "PAL/Qt/VulkanQtPlatform.h"
#else // Standalone game
#include "PAL/SDL/VulkanSDLPlatform.h"
#endif
