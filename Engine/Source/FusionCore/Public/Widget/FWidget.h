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

        virtual void PlaceSubWidgets();

        virtual void OnChildWidgetDestroyed(FWidget* child) {}

        void AddChild(FWidget* child);

        template<typename... TArgs> requires TMatchAllBaseClass<FWidget, TArgs...>::Value and (sizeof...(TArgs) > 0)
        Self& operator()(const TArgs&... childWidget)
        {
            using TupleType = std::tuple<const TArgs&...>;
            TupleType args = { childWidget... };
            
            constexpr_for<0, sizeof...(TArgs), 1>([&](auto i)
                {
                    using ArgTypeBase = std::tuple_element<i(), TupleType>::type;
                    using ArgType = std::remove_cvref_t<ArgTypeBase>;
                    if constexpr (TIsBaseClassOf<FWidget, ArgType>::Value)
                    {
                        this->AddChild(const_cast<ArgType*>(&std::get<i()>(args)));
                    }
                });
            
            return *this;
        }

        Vec2 GetIntrinsicSize() const { return intrinsicSize; }

        void MarkLayoutDirty();

        Vec2 GetComputedPosition() const { return computedPosition; }

        Vec2 GetComputedSize() const { return computedSize; }

    protected:

        virtual bool TryAddChild(FWidget* child) { return false; }

        virtual void OnFusionPropertyModified(const Name& propertyName) {}

        void OnAfterConstruct() override;

        void OnBeforeDestroy() override;

        virtual void Construct();

        //! @brief Computed position in parent widget's coordinate space
        Vec2 computedPosition{};

        Vec2 computedSize{};

        //! @brief Computed position in the fusion context's coordinate space
        //Vec2 globalComputedPosition{};

        Vec2 intrinsicSize{};

    private:  // - Fields -

        FIELD()
        FFusionContext* context = nullptr;

        FIELD()
        FWidget* parent = nullptr;

        // - Flags -


    public: // - Fusion Fields -

        FIELD()
        Vec4 m_Padding = Vec4();

        FIELD()
        Vec4 m_Margin = Vec4();

        FIELD()
        VAlign m_VAlign = VAlign::Auto;

        FIELD()
        HAlign m_HAlign = HAlign::Auto;

        FIELD()
        f32 m_MinWidth = 0;

        FIELD()
        f32 m_MaxWidth = NumericLimits<f32>::Infinity();

        FIELD()
        f32 m_MinHeight = 0;

        FIELD()
        f32 m_MaxHeight = NumericLimits<f32>::Infinity();

        FIELD()
        f32 m_FillWidth = 0.0f;

        FIELD()
        f32 m_FillHeight = 0.0f;

    public:  // - Fusion Properties -

        FUSION_LAYOUT_PROPERTY(Padding);

        FUSION_LAYOUT_PROPERTY(VAlign);
        FUSION_LAYOUT_PROPERTY(HAlign);

        FUSION_LAYOUT_PROPERTY(MinWidth);
        FUSION_LAYOUT_PROPERTY(MaxWidth);

        FUSION_LAYOUT_PROPERTY(MinHeight);
        FUSION_LAYOUT_PROPERTY(MaxHeight);

        FUSION_LAYOUT_PROPERTY(FillWidth);
        FUSION_LAYOUT_PROPERTY(FillHeight);

        FUSION_LAYOUT_PROPERTY(Margin);

        Self& Margin(f32 margin)
        {
            m_Margin = Vec4(margin, margin, margin, margin);
            return *this;
        }

        Self& Margin(f32 left, f32 top, f32 right, f32 bottom)
        {
            m_Margin = Vec4(left, top, right, bottom);
            return *this;
        }

        Self& Margin(f32 horizontal, f32 vertical)
        {
            m_Margin = Vec4(horizontal, vertical, horizontal, vertical);
            return *this;
        }

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
