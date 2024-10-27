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
                        .Width(140)
                    )
                )
                .RowHeight(25)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
            )
    	;

        if (scene)
        {
            treeViewModel->SetScene(scene);
            treeView->Model(treeViewModel);
        }

        treeView->ApplyStyleRecursively();
    }

    void SceneOutlinerTab::SetScene(CE::Scene* scene)
    {
        treeViewModel->SetScene(scene);
        treeView->Model(treeViewModel);
    }
    
}

