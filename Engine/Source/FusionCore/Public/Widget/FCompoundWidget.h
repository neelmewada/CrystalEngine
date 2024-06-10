#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FCompoundWidget : public FWidget
    {
        CE_CLASS(FCompoundWidget, FWidget)
    public:

        FCompoundWidget();

        void PrecomputeIntrinsicSize() override;

        void PlaceSubWidgets() override;

    protected:

        // Never call this function directly! Use AddChild() instead
        bool TryAddChild(FWidget* child) override;

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
