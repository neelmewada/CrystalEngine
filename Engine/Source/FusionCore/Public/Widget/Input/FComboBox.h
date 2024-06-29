#pragma once

namespace CE
{
    ENUM(Flags)
    enum class FComboBoxState
    {
	    Default = 0,
        Hovered = BIT(0),
        Pressed = BIT(1),
        Expanded = BIT(2)
    };
    ENUM_CLASS_FLAGS(FComboBoxState);

    CLASS()
    class FUSIONCORE_API FComboBox : public FStyledWidget
    {
        CE_CLASS(FComboBox, FStyledWidget)
    public:

        FComboBox();

        bool IsHovered() const { return EnumHasFlag(state, FComboBoxState::Hovered); }
        bool IsPressed() const { return EnumHasFlag(state, FComboBoxState::Pressed); }
        bool IsExpanded() const { return EnumHasFlag(state, FComboBoxState::Expanded); }

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsFocusEvents() const override { return true; }

    protected:

        void Construct() override;

        FIELD()
        FLabel* selectionText = nullptr;

        FIELD()
        FComboBoxState state = FComboBoxState::Default;

    protected: // - Fusion Fields -


    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER(Font, selectionText);
        FUSION_PROPERTY_WRAPPER(FontSize, selectionText);
        FUSION_PROPERTY_WRAPPER(FontFamily, selectionText);
        FUSION_PROPERTY_WRAPPER(Bold, selectionText);
        FUSION_PROPERTY_WRAPPER(Italic, selectionText);

        FUSION_WIDGET;
    };
    
}

#include "FComboBox.rtti.h"
