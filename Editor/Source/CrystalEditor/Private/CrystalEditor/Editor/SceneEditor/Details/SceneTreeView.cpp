#include "CrystalEditor.h"

namespace CE::Editor
{

    SceneTreeView::SceneTreeView()
    {

    }

    void SceneTreeView::Construct()
    {
        Super::Construct();

        (*this)
			.GenerateRowDelegate(MemberDelegate(&Self::GenerateRow, this))
            ;

        Style("TreeView");
    }

    FTreeViewRow& SceneTreeView::GenerateRow()
    {
        FTreeViewRow& row = FNew(FTreeViewRow);

        u32 numColumns = m_Model->GetColumnCount();

        for (int i = 0; i < numColumns; ++i)
        {
            row.AddCell(
				FNew(FTreeViewCell)
                .Text("Cell")
                .FontSize(13)
            );
        }

        return row;
    }

}

