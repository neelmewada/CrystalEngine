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
                .RowHeight(20)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
            )
    	;
    }
    
}

