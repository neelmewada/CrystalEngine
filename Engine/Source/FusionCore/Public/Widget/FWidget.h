#pragma once

namespace CE
{
    CLASS(Abstract)
    class FUSIONCORE_API FWidget : public Object
    {
        CE_CLASS(FWidget, Object)
    public:

        FWidget();

        virtual ~FWidget();

        // - Public API -

        FFusionContext* GetContext();

        virtual void PrecomputeIntrinsicSize();

        virtual void CalculateLayout(Vec2 availableSize);

        virtual void OnChildWidgetDestroyed(FWidget* child) {}

        void AddChild(FWidget* child);

        template<typename... TArgs> requires TMatchAllBaseClass<FWidget, TArgs...>::Value and (sizeof...(TArgs) > 0)
        Self& operator()(TArgs&... childWidget)
        {
            std::initializer_list<FWidget*> list = { (FWidget*)&childWidget... };
            for (FWidget* widget : list)
            {
                AddChild(widget);
            }
            return *this;
        }

    protected:

        virtual bool TryAddChild(FWidget* child) { return false; }

        virtual void OnFusionPropertyModified(const Name& propertyName) {}

        void OnAfterConstruct() override;

        void OnBeforeDestroy() override;

        virtual void Construct();

        //! @brief Computed position in parent widget's coordinate space
        Vec2 computedPosition{};

        Vec2 computedSize{};

        //! @brief Computed position is the owner fusion context's coordinate space
        Vec2 globalComputedPosition{};

    private:  // - Fields -

        FIELD()
        FFusionContext* context = nullptr;

        FIELD()
        FWidget* parent = nullptr;

        // - Flags -

        bool layoutDirty : 1 = true;

    public: // - Fusion Fields -

        FIELD()
        Vec4 m_Padding = Vec4();

        FIELD()
        VAlign m_VAlign = VAlign::Fill;

        FIELD()
        HAlign m_HAlign = HAlign::Fill;

        FIELD()
        f32 m_MinWidth = 0;

        FIELD()
        f32 m_MaxWidth = NumericLimits<f32>::Infinity();

        FIELD()
        f32 m_MinHeight = 0;

        FIELD()
        f32 m_MaxHeight = NumericLimits<f32>::Infinity();

    public:  // - Fusion Properties -

        FUSION_PROPERTY(Padding);

        FUSION_PROPERTY(VAlign);

        FUSION_PROPERTY(HAlign);

        FUSION_PROPERTY(MinWidth);
        FUSION_PROPERTY(MaxWidth);

        FUSION_PROPERTY(MinHeight);
        FUSION_PROPERTY(MaxHeight);

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

        template<typename TWidget> requires TIsBaseClassOf<FWidget, TWidget>::Value
        TWidget& As()
        {
            TWidget* cast = Object::CastTo<TWidget>(this);
            if (cast == nullptr)
            {
                throw FusionException(String::Format("Cannot cast object ({}) to type {}",
                    GetName(), GetStaticClass<TWidget>()->GetName()), 
                    this);
            }
            return (TWidget&)*cast;
        }

        Self& Name(const Name& newName)
        {
            this->SetName(newName);
            return *this;
        }

        template<typename TWidget> requires TIsBaseClassOf<FWidget, TWidget>::Value
        TWidget& Assign(TWidget*& out)
        {
            out = (TWidget*)this;
            return *out;
        }

    private:


        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FWidget.rtti.h"
