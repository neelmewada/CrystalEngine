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

        virtual void CalculateIntrinsicSize();

        virtual void PlaceSubWidgets();

        virtual void ApplyIntrinsicSizeConstraints();

        virtual void ApplySizeConstraints();

        void UpdateLocalTransform();

        virtual void OnChildWidgetDestroyed(FWidget* child) {}

        bool AddChild(FWidget* child);
        void RemoveChild(FWidget* child);

        virtual void ApplyStyle();

        template<typename... TArgs> requires TMatchAllBaseClass<FWidget, TArgs...>::Value and (sizeof...(TArgs) > 0)
        Self& operator()(const TArgs&... childWidget)
        {
            using TupleType = std::tuple<const TArgs&...>;
            TupleType args = { childWidget... };
            
            constexpr_for<0, sizeof...(TArgs), 1>([&](auto i)
                {
                    using ArgTypeBase = std::tuple_element_t<i(), TupleType>;
                    using ArgType = std::remove_cvref_t<ArgTypeBase>;
                    if constexpr (TIsBaseClassOf<FWidget, ArgType>::Value)
                    {
                        this->AddChild(const_cast<ArgType*>(&std::get<i()>(args)));
                    }
                });
            
            return *this;
        }

        virtual void SetContextRecursively(FFusionContext* context);

        Vec2 GetIntrinsicSize() const { return intrinsicSize; }

        void MarkLayoutDirty();

        //! @brief Mark the widget dirty for re-rendering
        void MarkDirty();

        Vec2 GetGlobalPosition() const;

        Vec2 GetComputedPosition() const { return computedPosition; }

        Vec2 GetComputedSize() const { return computedSize; }

        void SetComputedPosition(Vec2 pos) { computedPosition = pos; }

        void SetComputedSize(Vec2 size) { computedSize = size; }

        virtual void OnPaint(FPainter* painter);

        virtual void HandleEvent(FEvent* event);

        virtual FWidget* HitTest(Vec2 localMousePos);

        virtual bool ChildExistsRecursive(FWidget* child) { return this == child; }

        virtual bool ParentExistsRecursive(FWidget* parent);

        virtual bool SupportsMouseEvents() const { return false; }

        virtual bool SupportsKeyboardEvents() const { return false; }

    	virtual bool SupportsDragEvents() const { return false; }

        virtual bool SupportsFocusEvents() const { return false; }

        virtual void ClearStyle();

        bool IsFocused() const { return isFocused; }

        virtual bool IsSizeDependendentOnContent() const { return true; }
        virtual bool IsSizeDependendentOnParent() const { return true; }

        void Focus();
        void Unfocus();

    protected:

        virtual void OnGotFocus() {}
        virtual void OnLostFocus() {}

        virtual void OnAttachedToParent(FWidget* parent);
        virtual void OnDetachedFromParent(FWidget* parent);

        virtual bool TryAddChild(FWidget* child) { return false; }

        virtual bool TryRemoveChild(FWidget* child) { return false; }

        virtual void OnFusionPropertyModified(const Name& propertyName);

        void OnAfterConstruct() override;

        void OnBeforeDestroy() override;

        virtual void Construct();

        //! @brief Computed position in parent widget's coordinate space.
        Vec2 computedPosition;

        Vec2 computedSize;

        Vec2 intrinsicSize;

        //! @brief Global position of the widget, i.e. position in bottom-most context.
        Vec2 globalPosition;

        //! @brief Transformation matrix in parent widget's coordinate space.
        Matrix4x4 localTransform;

        FIELD()
        CE::Name styleKey;

    private:  // - Fields -

        FIELD()
        FFusionContext* context = nullptr;

        FIELD()
        FWidget* parent = nullptr;

        // - Flags -

        FIELD()
        bool isFocused = false;

    protected: // - Fusion Fields -

        FIELD(FusionProperty)
        FStyle* m_Style = nullptr;

    public:  // - Fusion Properties -

        FUSION_EVENT(ScriptEvent<void(FEvent*)>, OnEvent);

        FUSION_LAYOUT_PROPERTY(Vec4, Padding);
        FUSION_LAYOUT_PROPERTY(Vec4, Margin);

        FUSION_LAYOUT_PROPERTY(CE::VAlign, VAlign);
        FUSION_LAYOUT_PROPERTY(CE::HAlign, HAlign);

        FUSION_LAYOUT_PROPERTY(bool, Enabled);

        FUSION_LAYOUT_PROPERTY(f32, MinWidth);
        FUSION_LAYOUT_PROPERTY(f32, MaxWidth);

        FUSION_LAYOUT_PROPERTY(f32, MinHeight);
        FUSION_LAYOUT_PROPERTY(f32, MaxHeight);

        Self& Width(f32 width);
        Self& Height(f32 height);

        FUSION_LAYOUT_PROPERTY(f32, FillRatio);

        FUSION_PROPERTY(Vec2, Translation);
        FUSION_PROPERTY(f32, Angle);
        FUSION_PROPERTY(Vec2, Scale);

        Self& Style(FStyle* style);

        Self& Style(const CE::Name& styleKey);
        Self& Style(FStyleSet* styleSet, const CE::Name& styleKey);

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

        template<typename T>
        struct TValidate_IfTrue
        {
            using Traits = TFunctionTraits<T>;
            using ReturnType = typename Traits::ReturnType;
            using Arg0 = typename Traits::template Arg<0>::Type;

            static constexpr bool Value = (Traits::NumArgs == 1)
                and TIsSameType<ReturnType, void>::Value
        		and TIsBaseClassOf<FWidget, std::remove_cvref_t<Arg0>>::Value
        		and std::is_reference_v<Arg0>;
        };

    private:


        FUSION_WIDGET;
    };

    template<typename TWidget> requires !TIsAbstract<TWidget>::Value && TIsBaseClassOf<FWidget, TWidget>::Value
    TWidget& GetDefaultWidget()
    {
        CE_ASSERT(TWidget::Type()->CanBeInstantiated(), "The given widget class ({}) is an abstract class!", TWidget::Type()->GetName());
        return *GetMutableDefaults<TWidget>();
    }
    
} // namespace CE

#include "FWidget.rtti.h"
