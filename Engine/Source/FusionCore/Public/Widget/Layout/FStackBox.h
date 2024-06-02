#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FStackBoxSlot : public FSlot
    {
        CE_CLASS(FStackBoxSlot, FSlot)
    public:

        FStackBoxSlot();

        FUSION_TESTS;
    };

    ENUM()
    enum class FStackBoxDirection
    {
	    Horizontal,
        Vertical
    };
    ENUM_CLASS(FStackBoxDirection)

    CLASS()
    class FUSIONCORE_API FStackBox : public FWidget
    {
        CE_CLASS(FStackBox, FWidget)
    public:

        FStackBox();

        virtual ~FStackBox();

        // - Public API -

        u32 GetSlotCount() override;

        FSlot* GetSlot(u32 index) override;

        static FStackBoxSlot& Slot() { return *CreateObject<FStackBoxSlot>(nullptr, "StackBoxSlot"); }

        FWidget& operator+(FSlot& slot) override;

        FWidget& operator+(const FSlot& slot) override;

        bool RemoveSlot(FSlot* slot) override;

    protected:

        void Construct() override;

    private:  // - Fields -

        FIELD()
        Array<FStackBoxSlot*> m_Slots{};

        FIELD()
        FStackBoxDirection m_Direction = FStackBoxDirection::Horizontal;

    public:  // - Properties -

        FUSION_PROPERTY(Direction);


        FUSION_TESTS;
    };
    
} // namespace CE

#include "FStackBox.rtti.h"