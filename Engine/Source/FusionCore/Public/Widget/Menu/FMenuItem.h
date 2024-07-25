#pragma once

namespace CE
{
    class FMenuPopup;

    CLASS()
    class FUSIONCORE_API FMenuItem : public FStyledWidget
    {
        CE_CLASS(FMenuItem, FStyledWidget)
    public:

        FMenuItem();

    protected:

        void Construct() override final;

        FWidget* menuOwner = nullptr;

        FLabel* label = nullptr;

        FMenuPopup* subMenu = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER(Foreground, label);
        FUSION_PROPERTY_WRAPPER(FontSize, label);
        FUSION_PROPERTY_WRAPPER(FontFamily, label);

        FUSION_DATA_PROPERTY_WRAPPER(Text, label);

        FMenuItem& SubMenu(FMenuPopup& subMenu);

        FUSION_WIDGET;
        friend class FMenuPopup;
        friend class FMenuBar;
    };
    
}

#include "FMenuItem.rtti.h"
