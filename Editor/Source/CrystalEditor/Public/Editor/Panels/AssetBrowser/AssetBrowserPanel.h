#pragma once

namespace CE::Editor
{

    CLASS()
	class CRYSTALEDITOR_API AssetBrowserPanel : public EditorPanel
	{
		CE_CLASS(AssetBrowserPanel, EditorPanel)
	public:

		AssetBrowserPanel();
		virtual ~AssetBrowserPanel();

		void Construct() override;

	protected:

		FUNCTION()
		void OnGameContentTreeViewSelectionChanged(PathTreeNode* selectedNode);

		CContainerWidget* left = nullptr;
		CContainerWidget* right = nullptr;

		CTreeView* gameContentDirectoryView = nullptr;

		AssetBrowserTreeModel* folderModel = nullptr;
	};

} // namespace CE::Editor

#include "AssetBrowserPanel.rtti.h"
