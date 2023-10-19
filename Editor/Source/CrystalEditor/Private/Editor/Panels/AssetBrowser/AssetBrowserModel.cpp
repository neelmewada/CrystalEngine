#include "CrystalEditor.h"

namespace CE::Editor
{
	bool AssetBrowserTreeModel::HasHeader()
	{
		return false;
	}

	ClassType* AssetBrowserTreeModel::GetWidgetClass(const CModelIndex& index)
	{
		return GetStaticClass<CTreeItemView>();
	}

	u32 AssetBrowserTreeModel::GetRowCount(const CModelIndex& parent)
	{
		if (rootNode == nullptr)
			return 0;

		if (!parent.IsValid())
		{
			return 1;
		}

		PathTreeNode* node = (PathTreeNode*)parent.GetInternalData();
		if (node == nullptr)
		{
			return 0;
		}

		return node->children.GetSize();
	}

	u32 AssetBrowserTreeModel::GetColumnCount(const CModelIndex& parent)
	{
		return 1;
	}

	CModelIndex AssetBrowserTreeModel::GetParent(const CModelIndex& index)
	{
		if (!index.IsValid())
			return CModelIndex();

		auto curNode = (PathTreeNode*)index.GetInternalData();
		if (curNode == nullptr || curNode == rootNode)
			return CModelIndex();

		auto parentNode = curNode->parent;
		if (parentNode == nullptr)
			return CModelIndex();
		if (parentNode == rootNode)
			return CreateIndex(0, 0, rootNode);

		auto parentsParentNode = parentNode->parent;
		int parentsIndex = parentsParentNode->children.IndexOf(parentNode);

		return CreateIndex(parentsIndex, 0, parentNode);
	}

	CModelIndex AssetBrowserTreeModel::GetIndex(u32 row, u32 col, const CModelIndex& parent)
	{
		PathTreeNode* parentNode = nullptr;
		if (!parent.IsValid()) // Root node
			return CreateIndex(0, 0, rootNode);
		
		parentNode = (PathTreeNode*)parent.GetInternalData();
		if (parentNode == nullptr)
			return CModelIndex();
		if (row >= parentNode->children.GetSize())
			return CModelIndex();

		return CreateIndex(row, col, parentNode->children[row]);
	}

	void AssetBrowserTreeModel::SetData(const CModelIndex& index, CWidget* widget)
	{
		if (widget == nullptr || !widget->GetClass()->IsSubclassOf<CTreeItemView>())
			return;

		PathTreeNode* node = (PathTreeNode*)index.GetInternalData();
		CTreeItemView* itemWidget = (CTreeItemView*)widget;

		if (node == nullptr)
		{
			itemWidget->SetText("");
			return;
		}

		itemWidget->SetText(node->name.GetString());
		itemWidget->SetIcon("Icons/folder_32.png");
	}

	void AssetBrowserTreeModel::SetPathTreeRootNode(PathTreeNode* rootNode)
	{
		this->rootNode = rootNode;
	}

	void AssetBrowserTreeModel::OnIndexSelected(const CModelIndex& index)
	{
		PathTreeNode* node = (PathTreeNode*)index.GetInternalData();
		if (node == nullptr)
			return;

		OnSelectionChanged(node);
	}

} // namespace CE::Editor
