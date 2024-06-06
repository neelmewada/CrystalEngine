#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FWindowSlot : public FLayoutSlot
    {
        CE_CLASS(FWindowSlot, FLayoutSlot)
    public:
        FWindowSlot() = default;

    private:  // - Fields -

    public:  // - Functions -



    };

    CLASS()
    class FUSIONCORE_API FWindow : public FWidget
    {
        CE_CLASS(FWindow, FWidget)
    public:

        FWindow();

        SubClass<FLayoutSlot> GetSlotClass() const override { return FWindowSlot::StaticType(); }

        void Construct() override;

    public:

        Self& ChildSlot(FWidget& child)
        {
            m_ChildSlot->Child(child);
            return *this;
        }

    private: // - Fields -

        FIELD()
        FWindowSlot* m_ChildSlot = nullptr;

        FUSION_TESTS;
    };
    
}

#include "FWindow.rtti.h"
