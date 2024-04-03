#include "CoreWidgets.h"

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

	CModelIndex CBaseItemModel::CreateIndex(u32 row, u32 col, void* data)
	{
		return CModelIndex(row, col, this, data);
	}

	CModelIndex CBaseItemModel::FindIndex(void* internalData, const CModelIndex& parent)
	{
		if (internalData == nullptr)
			return CModelIndex();

		int rowCount = GetRowCount(parent);
		int colCount = GetColumnCount(parent);
		if (rowCount == 0 || colCount == 0)
			return CModelIndex();

		for (int r = 0; r < rowCount; r++)
		{
			for (int c = 0; c < colCount; c++)
			{
				CModelIndex cellIndex = GetIndex(c, r, parent);
				if (!cellIndex.IsValid())
					continue;
				if (cellIndex.GetInternalData() == internalData)
					return cellIndex;
				
				CModelIndex childSearch = FindIndex(internalData, cellIndex);
				if (childSearch.IsValid() && childSearch.GetInternalData() == internalData)
					return childSearch;
			}
		}

		return CModelIndex();
	}

	bool CBaseItemModel::IsIndexInParentChain(const CModelIndex& indexToSearch, const CModelIndex& bottomMostIndex)
	{
		if (!indexToSearch.IsValid() || !bottomMostIndex.IsValid())
			return false;

		if (bottomMostIndex == indexToSearch)
			return true;

		return IsIndexInParentChain(indexToSearch, GetParent(bottomMostIndex));
	}

} // namespace CE::Widgets
