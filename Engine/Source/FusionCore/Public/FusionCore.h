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
    friend class FRootContext;\
    friend class FNativeContext;\
    friend class FusionApplication;\
    friend class FCompoundWidget;\
    friend class FStyleManager;\
    friend class FusionRenderer;\
    friend class FPainter;

namespace CE
{
    // Forward Declarations
    class FStyleManager;

}

#include "FusionMacros.h"
#include "FusionDefines.h"
#include "Exception/FusionException.h"
#include "Font/FFontAtlas.h"
#include "Font/FFontManager.h"

#include "Event/FEvent.h"

#include "Style/FShape.h"
#include "Style/FPen.h"
#include "Style/FBrush.h"
#include "Style/FFont.h"

#include "Application/FTimer.h"
#include "Application/FusionApplication.h"
#include "Rendering/FusionRenderer.h"
#include "Rendering/FPainter.h"
#include "Context/FFusionContext.h"
#include "Context/FRootContext.h"
#include "Context/FNativeContext.h"

#include "Layout/LayoutTypes.h"
#include "Layout/FLayoutManager.h"

#include "Style/FStyle.h"
#include "Style/FButtonStyle.h"
#include "Style/FTextInputStyle.h"
#include "Style/FComboBoxStyle.h"
#include "Style/FStyleSet.h"
#include "Style/FStyleManager.h"

#include "Data/FPropertyBinding.h"
#include "Data/FDataModel.h"

#include "Widget/FWidget.h"
#include "Widget/FTerminalWidget.h"
#include "Widget/FLabel.h"
#include "Widget/FCompoundWidget.h"
#include "Widget/FStyledWidget.h"
#include "Widget/FContainerWidget.h"

#include "Widget/Layout/FStackBox.h"
#include "Widget/Layout/FOverlayStack.h"

#include "Widget/FPopup.h"
#include "Widget/FImage.h"
#include "Widget/FButton.h"
#include "Widget/FTextButton.h"
#include "Widget/Input/FTextInput.h"
#include "Widget/ComboBox/FComboBoxPopup.h"
#include "Widget/ComboBox/FComboBoxItem.h"
#include "Widget/ComboBox/FComboBox.h"

#include "Window/FTitleBar.h"
#include "Window/FWindow.h"

