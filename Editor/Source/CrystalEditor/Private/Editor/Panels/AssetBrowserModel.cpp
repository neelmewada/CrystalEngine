#include "CrystalEditor.h"

namespace CE::Editor
{
	bool AssetBrowserTreeModel::HasHeader()
	{
		return false;
	}

	u32 AssetBrowserTreeModel::GetRowCount(const CModelIndex& parent)
	{
		return 0;
	}

	u32 AssetBrowserTreeModel::GetColumnCount(const CModelIndex& parent)
	{
		return 1;
	}

	CModelIndex AssetBrowserTreeModel::GetParent(const CModelIndex& index)
	{
		return CModelIndex();
	}

	CModelIndex AssetBrowserTreeModel::GetIndex(u32 row, u32 col, const CModelIndex& parent)
	{
		return CModelIndex();
	}

	void AssetBrowserTreeModel::SetData(const CModelIndex& index, CAbstractItemCell* itemWidget)
	{

	}

} // namespace CE::Editor
