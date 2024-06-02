#pragma once

namespace CE
{
    class FWidget;

    CLASS()
    class FUSIONCORE_API FSlot : public Object
    {
        CE_CLASS(FSlot, Object)
    public:

        FSlot() = default;

    protected:  // - Fields -

        FIELD()
        FWidget* m_Owner = nullptr;

        FIELD()
        FWidget* m_Child = nullptr;

        FIELD()
        Vec4 m_Padding{};

        void OnBeforeDestroy() override;

    public:  // - Properties -

        //! @brief Do NOT use this method directly!
        void SetOwner(FWidget* owner);

        //! @brief Do NOT use this method directly!
        void SetChild(FWidget* newChild);

        FUSION_PROPERTY(Padding);

        FWidget* GetOwner() const { return m_Owner; }

        Self& Padding(f32 padding)
        {
            m_Padding = Vec4(1, 1, 1, 1) * padding;
            return *this;
        }

        Self& Padding(f32 horizontal, f32 vertical)
        {
            m_Padding = Vec4(horizontal, vertical, horizontal, vertical);
            return *this;
        }

        Self& Padding(f32 left, f32 top, f32 right, f32 bottom)
        {
            m_Padding = Vec4(left, top, right, bottom);
            return *this;
        }

        FWidget* GetChild() const { return m_Child; }

        virtual Self& Child(FWidget& child);

        Self& operator()(FWidget& child)
        {
            return Child(child);
        }

        FUSION_TESTS;
    };
    
} // namespace CE

#include "FSlot.rtti.h"