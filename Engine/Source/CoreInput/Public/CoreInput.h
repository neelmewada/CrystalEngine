#pragma once

#include "Core.h"
#include "CoreApplication.h"

#include "Input/CoreInputTypes.h"

#include "Input/PAL/Common/PlatformInput.h"
#if PAL_TRAIT_SDL_SUPPORTED
#include "Input/PAL/SDL/SDLPlatformInput.h"
#endif

#include "Input/InputManager.h"

namespace CE
{
    
}