#include "Fusion.h"

namespace CE
{
	SIZE_T FModelIndex::GetHash() const
	{
        return GetCombinedHashes({ (SIZE_T)GetRow(), (SIZE_T)GetColumn(), (SIZE_T)GetDataPtr() });
	}

	bool FModelIndex::operator==(const FModelIndex& other) const
	{
        return GetHash() == other.GetHash();
	}

	bool FModelIndex::operator!=(const FModelIndex& other) const
	{
        return !operator==(other);
	}

    FAbstractItemModel::FAbstractItemModel()
    {
        
    }

    FAbstractItemModel::~FAbstractItemModel()
    {
        
    }

    FModelIndex FAbstractItemModel::CreateIndex(u32 row, u32 col, void* data)
    {
        FModelIndex index;
        index.row = row;
        index.col = col;
        index.data = data;
        index.model = this;

        return index;
    }

} // namespace CE

