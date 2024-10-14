#include "Fusion.h"

namespace CE
{

    FTreeView::FTreeView()
    {

    }

    void FTreeView::Construct()
    {
        Super::Construct();

        Child(
            FNew(FVerticalStack)
            .VAlign(VAlign::Fill)
            .HAlign(HAlign::Fill)
            (
                FAssignNew(FStyledWidget, headerContainer)
                .HAlign(HAlign::Fill),

                FAssignNew(FTreeViewContainer, container)
                .TreeView(this)
                .HAlign(HAlign::Fill)
                .FillRatio(1.0f)
            )
        );
    }

    void FTreeView::OnFusionPropertyModified(const CE::Name& propertyName)
    {
	    Super::OnFusionPropertyModified(propertyName);

        static const CE::Name model = "Model";

        if (propertyName == model)
        {
            MarkLayoutDirty();
        }
    }

}

