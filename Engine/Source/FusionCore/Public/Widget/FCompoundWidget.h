#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FCompoundWidgetSlot : public FSlot
    {
        CE_CLASS(FCompoundWidgetSlot, FSlot)
    public:

        FCompoundWidgetSlot() = default;


    };

    CLASS()
    class FUSIONCORE_API FCompoundWidget : public FWidget
    {
        CE_CLASS(FCompoundWidget, FWidget)
    public:

        FCompoundWidget();

        u32 GetSlotCount() override { return 1; }

        FSlot* GetSlot(u32 index) override { return m_ChildSlot; }

        bool RemoveSlot(FSlot* slot) override;

    protected:

        void Construct() override;

        Self& ChildSlot(FWidget& child)
        {
            m_ChildSlot->Child(child);
            return *this;
        }

    private:  // - Fields -

        FIELD()
        FCompoundWidgetSlot* m_ChildSlot = nullptr;

        FUSION_TESTS;
    };
    
}

#include "FCompoundWidget.rtti.h"
