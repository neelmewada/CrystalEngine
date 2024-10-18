#pragma once

#include "Core.h"
#include "CoreApplication.h"
#include "CoreRHI.h"
#include "CoreRPI.h"

#if PAL_TRAIT_VULKAN_SUPPORTED
#	include "VulkanRHI.h"
#endif

#include "System.h"
#include "GameSystem.h"

#include "cxxopts.hpp"

// GameLauncher includes
#include "GameLoop.h"


