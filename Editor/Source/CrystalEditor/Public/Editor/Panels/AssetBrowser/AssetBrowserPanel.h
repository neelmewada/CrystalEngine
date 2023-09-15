#pragma once

namespace CE::Editor
{
	class AssetItemWidget;

	namespace Private
	{
		STRUCT()
		struct AssetBrowserPanelPrefs
		{
			CE_STRUCT(AssetBrowserPanelPrefs)
		public:

			static String PrefsKey();

			FIELD()
			u32 thumbnailSize = 1;

			FIELD()
			Name initialPath = "";
		};
	}

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

		void OnBeforeDestroy() override;
		
		void UpdateContentView();

	protected:

		void StorePrefs();
		
		void HandleEvent(CEvent* event) override;

		void SetSelectedItem(int index);
		void SetSelectedItem(AssetItemWidget* item);

		void HandleOpenAsset(AssetItemWidget* item);

		FUNCTION()
		void OnThumbnailSizeMenuItemClicked(CMenuItem* menuItem);

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

		Private::AssetBrowserPanelPrefs prefs{};

		DelegateHandle assetRegistryModified = 0;
	};

} // namespace CE::Editor

#include "AssetBrowserPanel.rtti.h"
