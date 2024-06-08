#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FContainerWidget : public FWidget
    {
        CE_CLASS(FContainerWidget, FWidget)
    public:

        FContainerWidget();

        bool TryAddChild(FWidget* child) override;

        u32 GetChildCount() const { return children.GetSize(); }

        FWidget* GetChild(u32 index) const { return children[index]; }

    protected: // - Fields -

        FIELD()
        Array<FWidget*> children{};

    public:

        

        FUSION_TESTS;
    };

} // namespace CE

#include "FContainerWidget.rtti.h"