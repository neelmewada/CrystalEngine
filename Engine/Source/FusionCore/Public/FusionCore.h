#pragma once

#include "Core.h"
#include "CoreApplication.h"
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
    friend class FStackBox;\
    friend class FWindow;\
    friend class FFusionContext;\
    friend class FNativeContext;\
    friend class FusionApplication;\
    friend class FCompoundWidget;\
    friend class FStyleManager;\
    friend class FusionRenderer;

namespace CE
{
    // Forward Declarations
    class FStyleManager;

}

#include "FusionMacros.h"
#include "Exception/FusionException.h"

#include "Application/FusionApplication.h"
#include "Rendering/FusionRenderer.h"
#include "Rendering/FPainter.h"
#include "Context/FFusionContext.h"
#include "Context/FNativeContext.h"

#include "Layout/LayoutTypes.h"
#include "Layout/FLayoutManager.h"

#include "Style/FShape.h"
#include "Style/FBrush.h"
#include "Style/FStyle.h"
#include "Style/FButtonStyle.h"
#include "Style/FStyleManager.h"

#include "Widget/FWidget.h"
#include "Widget/FCompoundWidget.h"
#include "Widget/FStyledWidget.h"
#include "Widget/FContainerWidget.h"
#include "Widget/FButton.h"

#include "Window/FWindow.h"

#include "Widget/FNullWidget.h"
#include "Widget/Layout/FStackBox.h"

