#pragma once

namespace CE
{
    class FComboBox;

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

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsFocusEvents() const override { return true; }

    protected:

        void Construct() override final;

        void HandleEvent(FEvent* event) override;

        FIELD()
        FHorizontalStack* contentBox = nullptr;

        FIELD()
        FComboBoxItemState state = FComboBoxItemState::None;

        FIELD()
        FLabel* label = nullptr;

        FIELD()
        FComboBox* comboBox = nullptr;

    protected: // - Fusion Fields -


    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER(ContentHAlign, contentBox);
        FUSION_PROPERTY_WRAPPER(ContentVAlign, contentBox);

        FUSION_PROPERTY_WRAPPER(Text, label);
        FUSION_PROPERTY_WRAPPER(Font, label);
        FUSION_PROPERTY_WRAPPER(FontSize, label);

        FUSION_WIDGET;
        friend class FComboBox;
    };
    
}

#include "FComboBoxItem.rtti.h"
