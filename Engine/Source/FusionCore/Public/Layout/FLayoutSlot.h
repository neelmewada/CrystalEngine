#pragma once

namespace CE
{
    class FWidget;

    CLASS()
    class FUSIONCORE_API FLayoutSlot : public Object
    {
        CE_CLASS(FLayoutSlot, Object)
    public:

        FLayoutSlot() = default;

    protected:  // - Fusion Fields -

        // Fusion exposed fields are always prefixed with 'm_'

        FIELD()
        FWidget* m_Owner = nullptr;

        FIELD()
        FWidget* m_Child = nullptr;

        FIELD()
        Vec4 m_Padding{};

        FIELD()
        VAlign m_VAlign = VAlign::Fill;

        FIELD()
        HAlign m_HAlign = HAlign::Fill;

        FIELD()
        f32 m_MinWidth = 0.0f;

        FIELD()
        f32 m_MinHeight = 0.0f;

        FIELD()
        f32 m_MaxWidth = NumericLimits<f32>::Max();

        FIELD()
        f32 m_MaxHeight = NumericLimits<f32>::Max();

        Vec2 computedPosition{};
        Vec2 computedSize{};

        void OnBeforeDestroy() override;

        virtual void OnFusionPropertyModified(const Name& propertyName) {}

    public:  // - Fusion Properties -

        void SetComputedPosition(Vec2 pos) { computedPosition = pos; }

        void SetComputedSize(Vec2 size) { computedSize = size; }

        //! @brief Do NOT use this method directly!
        void SetOwner(FWidget* owner);

        //! @brief Do NOT use this method directly!
        void SetChild(FWidget* newChild);

        FWidget* GetChild() const { return m_Child; }

        FWidget* GetOwner() const { return m_Owner; }

        FUSION_PROPERTY(Padding);

        FUSION_PROPERTY(VAlign);

        FUSION_PROPERTY(HAlign);

        FUSION_PROPERTY(MinWidth);

        FUSION_PROPERTY(MinHeight);

        FUSION_PROPERTY(MaxWidth);

        FUSION_PROPERTY(MaxHeight);

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

        virtual Self& Child(FWidget& child);

        Self& operator()(FWidget& child)
        {
            return Child(child);
        }

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FLayoutSlot.rtti.h"