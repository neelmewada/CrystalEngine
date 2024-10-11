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

        bool IsHovered() const { return isHovered; }

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsFocusEvents() const override { return true; }

        void OpenSubMenu();

    protected:

        void Construct() override final;

        void HandleEvent(FEvent* event) override;

        FWidget* menuOwner = nullptr;

        FLabel* label = nullptr;

        FMenuPopup* subMenu = nullptr;

        bool isHovered = false;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER(Foreground, label);
        FUSION_PROPERTY_WRAPPER(FontSize, label);
        FUSION_PROPERTY_WRAPPER(FontFamily, label);

        FUSION_DATA_PROPERTY_WRAPPER(Text, label);

        FUSION_EVENT(FVoidEvent, OnClick);

        FMenuItem& SubMenu(FMenuPopup& subMenu);

        FUSION_WIDGET;
        friend class FMenuPopup;
        friend class FMenuBar;
    };
    
}

#include "FMenuItem.rtti.h"
