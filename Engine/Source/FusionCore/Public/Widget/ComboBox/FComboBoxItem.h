#pragma once

namespace CE
{
    ENUM(Flags)
    enum class FComboBoxItemState : u8
    {
	    None = 0,
        Hovered = BIT(0),
        Selected = BIT(2),
    };
    ENUM_CLASS_FLAGS(FComboBoxItemState);

    CLASS()
    class FUSIONCORE_API FComboBoxItem : public FStyledWidget
    {
        CE_CLASS(FComboBoxItem, FStyledWidget)
    public:

        FComboBoxItem();

        // - Public API -

        bool IsHovered() const { return EnumHasFlag(state, FComboBoxItemState::Hovered); }
        bool IsSelected() const { return EnumHasFlag(state, FComboBoxItemState::Selected); }

    protected:

        void Construct() override final;

        FIELD()
        FHorizontalStack* contentBox = nullptr;

        FIELD()
        FComboBoxItemState state = FComboBoxItemState::None;

        FIELD()
        FLabel* label = nullptr;

    protected: // - Fusion Fields -


    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER(ContentHAlign, contentBox);
        FUSION_PROPERTY_WRAPPER(ContentVAlign, contentBox);

        FUSION_PROPERTY_WRAPPER(Text, label);
        FUSION_PROPERTY_WRAPPER(Font, label);
        FUSION_PROPERTY_WRAPPER(FontSize, label);

        FUSION_WIDGET;
    };
    
}

#include "FComboBoxItem.rtti.h"
