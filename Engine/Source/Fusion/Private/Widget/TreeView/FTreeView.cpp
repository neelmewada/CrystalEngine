#include "Fusion.h"

namespace CE
{

    FTreeView::FTreeView()
    {
        m_ScrollBarWidth = 8;
        m_ScrollBarShape = FRoundedRectangle(4);
        m_ScrollBarMargin = 2.5f;

        m_VerticalScrollSensitivity = 15.0f;

        m_AutoHeight = false;
        m_MinHeight = 30;
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

                FAssignNew(FStyledWidget, containerStyle)
                .ClipChildren(true)
                .HAlign(HAlign::Fill)
                .FillRatio(1.0f)
                (
                    FAssignNew(FTreeViewContainer, container)
                    .TreeView(this)
                    .HAlign(HAlign::Fill)
                    .FillRatio(1.0f)
                )
            )
        );
    }

    void FTreeView::OnFusionPropertyModified(const CE::Name& propertyName)
    {
	    Super::OnFusionPropertyModified(propertyName);

        static const CE::Name model = "Model";

        if (propertyName == model)
        {
            // TODO: Do model loading
            container->OnModelUpdate();
            
            MarkLayoutDirty();
        }
    }

    FTreeView::Self& FTreeView::Header(FTreeViewHeader& header)
    {
        this->header = &header;
        header.treeView = this;
        
        header
			.HAlign(HAlign::Fill)
			.VAlign(VAlign::Fill)
    	;

        headerContainer->Child(header);

        ApplyStyle();
        return *this;
    }

    void FTreeView::OnModelUpdate()
    {
        container->OnModelUpdate();
    }

}

