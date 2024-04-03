#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CModelIndex::CModelIndex(u32 row, u32 col, CBaseItemModel* model, void* data)
	    : row(row), col(col), model(model), data(data)
    {
	    
    }

	bool CModelIndex::IsValid() const
	{
		return model != nullptr;
	}

	CModelIndex CModelIndex::GetParent() const
	{
		if (model == nullptr)
			return {};
		return model->GetParent(*this);
	}

	bool CModelIndex::operator==(const CModelIndex& other) const
	{
		return row == other.row && col == other.col && model == other.model && data == other.data;
	}

	bool CModelIndex::operator!=(const CModelIndex& other) const
	{
		return !(*this == other);
	}

    
} // namespace CE::Widgets
