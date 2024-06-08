#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FCompoundWidget : public FWidget
    {
        CE_CLASS(FCompoundWidget, FWidget)
    public:

        FCompoundWidget();

    protected:

        // Never call this function directly! Use AddChild() instead
        bool TryAddChild(FWidget* child) override;

    private:  // - Fusion Fields -

        FIELD()
        FWidget* m_Child = nullptr;

    public: // - Fusion Properties -

        FWidget* GetChild() const { return m_Child; }

        Self& Child(FWidget& childWidget)
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
    };
    
}

#include "FCompoundWidget.rtti.h"
