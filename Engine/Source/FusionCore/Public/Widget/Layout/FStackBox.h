#pragma once


namespace CE
{

    CLASS()
    class FUSIONCORE_API FStackBoxSlot : public FLayoutSlot
    {
        CE_CLASS(FStackBoxSlot, FLayoutSlot)
    public:

        FStackBoxSlot();

    private: // - Fusion Fields -

        FIELD()
        bool m_AutoSize = false;

        FIELD()
        f32 m_FillRatio = 0.0f;

    public:

        FUSION_PROPERTY(AutoSize);

        FUSION_PROPERTY(FillRatio);

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

        SubClass<FLayoutSlot> GetSlotClass() const override { return FStackBoxSlot::StaticType(); }

        u32 GetSlotCount() override;

        FLayoutSlot* GetSlot(u32 index) override;

        static FStackBoxSlot& Slot() { return *CreateObject<FStackBoxSlot>(nullptr, "StackBoxSlot"); }

        FWidget& operator+(FLayoutSlot& slot) override;

        FWidget& operator+(const FLayoutSlot& slot) override;

        bool RemoveLayoutSlot(FLayoutSlot* slot) override;

        Vec2 PrecomputeLayoutSize() override;

    protected:

        void Construct() override;

    private:  // - Fusion Fields -

        FIELD()
        Array<FStackBoxSlot*> m_Slots{};

        FIELD()
        FStackBoxDirection m_Direction = FStackBoxDirection::Horizontal;

        FIELD()
        Vec4 m_Padding{};

    public:  // - Fusion Properties -

        FUSION_PROPERTY(Direction);

        FUSION_PROPERTY(Padding);

        Self& Padding(f32 padding)
        {
            m_Padding = Vec4(1, 1, 1, 1) * padding;
            return *this;
        }

        Self& Padding(f32 left, f32 top, f32 right, f32 bottom)
        {
            m_Padding = Vec4(left, top, right, bottom);
            return *this;
        }

        Self& Padding(f32 horizontal, f32 vertical)
        {
            m_Padding = Vec4(horizontal, vertical, horizontal, vertical);
            return *this;
        }

        FUSION_TESTS;
    };
    
} // namespace CE

#include "FStackBox.rtti.h"