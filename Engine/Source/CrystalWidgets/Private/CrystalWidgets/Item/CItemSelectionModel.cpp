#include "CrystalWidgets.h"

namespace CE::Widgets
{

	bool CItemSelection::IsSelected(const CModelIndex& index) const
	{
        if (!index.IsValid())
            return false;

        CModelIndex indexParent = index.GetParent();

        for (int i = 0; i < selectionRanges.GetSize(); ++i)
        {
            const CModelIndex& start = selectionRanges[i].start;
            const CModelIndex& end = selectionRanges[i].end;

            if (index.GetRow() >= start.GetRow() && index.GetRow() <= end.GetRow() &&
                index.GetColumn() >= start.GetColumn() && index.GetColumn() <= end.GetColumn() &&
                indexParent == start.GetParent() && indexParent == end.GetParent())
            {
                return true;
            }
        }

        return false;
	}

    CItemSelectionModel::CItemSelectionModel()
    {
	    
    }

    CItemSelectionModel::~CItemSelectionModel()
    {
	    
    }

    void CItemSelectionModel::Select(const CModelIndex& start, const CModelIndex& end)
    {
        selection.Select(start, end);
        OnSelectionChanged();
    }

    void CItemSelectionModel::Select(const CModelIndex& index)
    {
        selection.Select(index, index);
        OnSelectionChanged();
    }

    void CItemSelectionModel::Clear()
    {
        selection.ClearAll();
        OnSelectionChanged();
    }

} // namespace CE::Widgets
