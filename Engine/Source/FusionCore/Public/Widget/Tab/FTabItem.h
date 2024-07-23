#pragma once

namespace CE
{
    class FTabView;

    ENUM(Flags)
    enum class FTabItemFlags : u8
    {
	    None = 0,
        Active = BIT(0),
        Hovered = BIT(1)
    };
    ENUM_CLASS_FLAGS(FTabItemFlags);

    CLASS()
    class FUSIONCORE_API FTabItem : public FStyledWidget
    {
        CE_CLASS(FTabItem, FStyledWidget)
    public:

        FTabItem();

        bool IsActive() const { return EnumHasFlag(itemFlags, FTabItemFlags::Active); }
        bool IsHovered() const { return EnumHasFlag(itemFlags, FTabItemFlags::Hovered); }

    protected:

        FTabItemFlags itemFlags = FTabItemFlags::None;

        FIELD()
        FTabView* tabView = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(FWidget*, ContentWidget);

        Self& ContentWidget(FWidget& value)
        {
            return ContentWidget(&value);
        }

        FUSION_WIDGET;
        friend class FTabView;
    };

    CLASS()
    class FUSIONCORE_API FLabelTabItem : public FTabItem
    {
        CE_CLASS(FLabelTabItem, FTabItem)
    public:

        FLabelTabItem();

    protected:

        void Construct() override final;

        FIELD()
        FLabel* label = nullptr;


    public: // - Fusion Properties -

        FUSION_PROPERTY_WRAPPER(Foreground, label);
        FUSION_PROPERTY_WRAPPER(FontSize, label);
        FUSION_PROPERTY_WRAPPER(FontFamily, label);
        FUSION_PROPERTY_WRAPPER(Bold, label);

        FUSION_DATA_PROPERTY_WRAPPER(Text, label);


        FUSION_WIDGET;
    };
    
}

#include "FTabItem.rtti.h"
