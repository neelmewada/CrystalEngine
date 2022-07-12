#pragma once

#define SDL_MAIN_HANDLED

#include "EngineDefs.h"

// -- Math --
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// -- IO --
#include "IO/Directories.h"
#include "IO/FileIO.h"
#include "IO/FileManager.h"

// -- RENDERING --
#include "Rendering/Types.h"
#include "Rendering/IEngineContext.h"
#include "Rendering/IDeviceContext.h"
#include "Rendering/IShader.h"
#include "Rendering/ISwapChain.h"
#include "Rendering/IRenderContext.h"
#include "Rendering/IBuffer.h"
#include "Rendering/ITexture.h"
#include "Rendering/IGraphicsPipelineState.h"
#include "Rendering/EngineFactoryVk.h"

// -- UTILS --

// -- APPLICATION --
#include "ApplicationBase.h"


#define MAKE_VERSION(variant, major, minor, patch) \
    ((((uint32_t)(variant)) << 29) | (((uint32_t)(major)) << 22) | (((uint32_t)(minor)) << 12) | ((uint32_t)(patch)))

#define ENGINE_VERSION MAKE_VERSION(0, 0, 1, 0)
#define ENGINE_NAME "VoxEngine"

