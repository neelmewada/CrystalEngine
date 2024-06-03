#pragma once

#include "Core.h"
#include "CoreRPI.h"

#if PAL_TRAIT_BUILD_TESTS

// Test Classes
class FusionCore_Construction_Test;
class FusionCore_Layout_Test;
#define FUSION_TESTS \
    friend class FusionCore_Construction_Test;\
    friend class FusionCore_Layout_Test;

#else

#define FUSION_TESTS

#endif

#define FUSION_FRIENDS FUSION_TESTS\
    friend class FWidget;\
    friend class FFusionContext;\
    friend class FNativeContext;\
    friend class FusionApplication;\
    friend class FSlot;

#include "FusionMacros.h"
#include "Exception/FusionException.h"

#include "Application/FusionApplication.h"
#include "Application/FusionContext.h"
#include "Application/FNativeContext.h"
#include "Rendering/FusionRenderer.h"

#include "Layout/LayoutTypes.h"
#include "Layout/FLayoutManager.h"
#include "Layout/FSlot.h"

#include "Widget/FWidget.h"
#include "Window/FWindow.h"

#include "Widget/FNullWidget.h"
#include "Widget/Layout/FStackBox.h"
#include "Widget/FCompoundWidget.h"


namespace CE
{
    
}