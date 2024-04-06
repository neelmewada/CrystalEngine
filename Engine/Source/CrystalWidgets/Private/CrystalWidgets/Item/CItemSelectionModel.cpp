#include "CrystalWidgets.h"

namespace CE::Widgets
{

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
