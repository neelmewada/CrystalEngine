#pragma once

namespace CE::Editor
{
	class AssetItemWidget;

    CLASS()
	class CRYSTALEDITOR_API AssetBrowserPanel : public EditorPanel
	{
		CE_CLASS(AssetBrowserPanel, EditorPanel)
	public:

		AssetBrowserPanel();
		virtual ~AssetBrowserPanel();

		void Construct() override;

		/// Sets current assets directory to be displayed in the panel
		void SetCurrentAssetDirectory(const Name& path);

	private:

		void UpdateContentView();

	protected:
		
		void HandleEvent(CEvent* event) override;

		void SetSelectedItem(int index);
		void SetSelectedItem(AssetItemWidget* item);

		FUNCTION()
		void OnGameContentTreeViewSelectionChanged(PathTreeNode* selectedNode);

		FUNCTION()
		void OnEngineContentTreeViewSelectionChanged(PathTreeNode* selectedNode);

		Array<AssetItemWidget*> assetItems{};
		Array<AssetItemWidget*> freeAssetItems{};

		FIELD(ReadOnly)
		CContainerWidget* left = nullptr;

		FIELD(ReadOnly)
		CContainerWidget* right = nullptr;

		FIELD(ReadOnly)
		CContainerWidget* assetGridView = nullptr;

		FIELD(ReadOnly)
		CTreeView* gameContentDirectoryView = nullptr;

		FIELD(ReadOnly)
		AssetBrowserTreeModel* folderModel = nullptr;

		FIELD(ReadOnly)
		int selectedAssetItem = -1;

		FIELD(ReadOnly)
		Name selectedDirectoryPath{};
	};

} // namespace CE::Editor

#include "AssetBrowserPanel.rtti.h"
