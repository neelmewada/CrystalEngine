#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneOutlinerTab::SceneOutlinerTab()
    {

    }

    void SceneOutlinerTab::Construct()
    {
        Super::Construct();
        
        (*this)
			.Title("Scene Outliner")
			.Content(
                FAssignNew(SceneTreeView, treeView)
                .Header(
                    FNew(FTreeViewHeader)
                    .Columns(
                        FNew(FTreeViewHeaderColumn)
                        .Title("Name")
                        .FillRatio(1.0f),

                        FNew(FTreeViewHeaderColumn)
                        .Title("Type")
                        .Width(80)
                    )
                )
                .RowHeight(20)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
            )
    	;

        treeView->ApplyStyleRecursively();
    }
    
}

