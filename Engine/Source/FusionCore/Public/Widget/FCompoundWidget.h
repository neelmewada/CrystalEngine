#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FCompoundWidget : public FWidget
    {
        CE_CLASS(FCompoundWidget, FWidget)
    public:

        FCompoundWidget();

        void CalculateIntrinsicSize() override;

        void PlaceSubWidgets() override;

        void OnPaint(FPainter* painter) override;

        void SetContextRecursively(FFusionContext* context) override;

        FWidget* HitTest(Vec2 localMousePos) override;

        bool ChildExistsRecursive(FWidget* child) override;

        void HandleEvent(FEvent* event) override;

        void ClearStyle() override;

    protected:

        // Never call this function directly! Use AddChild() instead
        bool TryAddChild(FWidget* child) override;

        // Never call this function directly! Use RemoveChild() instead
        bool TryRemoveChild(FWidget* child) override;

        void OnChildWidgetDestroyed(FWidget* child) override;

    private:  // - Private Fields -

        FIELD()
        FWidget* m_Child = nullptr;

    protected:  // - Fusion Fields -


    public: // - Fusion Properties -


        FWidget* GetChild() const { return m_Child; }

        virtual Self& Child(FWidget& childWidget)
        {
            AddChild(&childWidget);
            return *this;
        }

        Self& operator()(FWidget& childWidget)
        {
            AddChild(&childWidget);
            return *this;
        }

        FUSION_TESTS;
        FUSION_WIDGET;
    };
    
}

#include "FCompoundWidget.rtti.h"
