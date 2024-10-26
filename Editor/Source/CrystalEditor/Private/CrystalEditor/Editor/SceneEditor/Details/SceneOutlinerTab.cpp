#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneOutlinerTab::SceneOutlinerTab()
    {

    }

    void SceneOutlinerTab::Construct()
    {
        Super::Construct();

        CE::Scene* scene = gEngine->GetActiveScene();

        treeViewModel = CreateObject<SceneTreeViewModel>(this, "TreeViewModel");
        treeViewModel->SetScene(scene);
        
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
                .Model(treeViewModel)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
            )
    	;

        treeView->ApplyStyleRecursively();
    }
    
}

