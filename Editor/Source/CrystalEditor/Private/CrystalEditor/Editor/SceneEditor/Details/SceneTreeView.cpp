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

        row.Cells(
            FNew(FTreeViewCell)
            .Text("Name")
            .ArrowEnabled(true)
            .FontSize(13),

            FNew(FTreeViewCell)
            .Text("Type")
            .FontSize(13)
            .Foreground(Color::RGBA(255, 255, 255, 140))
            .ArrowEnabled(false)
        );

        return row;
    }

}

