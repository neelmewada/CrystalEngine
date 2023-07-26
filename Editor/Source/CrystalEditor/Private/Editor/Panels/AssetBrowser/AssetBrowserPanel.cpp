#include "CrystalEditor.h"

static const CE::String css = R"(

AssetBrowserPanel {
	padding: 0 25px 0 0;
	position: absolute;
	top: 0; left: 0; bottom: 0; right: 0;
	flex-direction: row;
}

#LeftContainer {
	width: 25%;
	min-width: 200px;
	height: 100%;
	padding: 10px 5px 20px 5px;
	background: rgb(26, 26, 26);
	flex-direction: column;
	justify-content: flex-start;
}

#RightContainer {
	padding: 0px 0px;
	width: 75%;
	height: 100%;
	flex-direction: column;
	justify-content: flex-start;
	align-items: stretch;
}

#GameContentSection {
	
}

CTreeView {
	padding: 0px 10px;
}

CTreeView > CTreeItemView {
	text-align: middle-left;
	font-size: 18px;
	height: 22px;
}

CTreeView > CTreeItemView:hovered {
	background: rgb(43, 50, 58);
}

CTreeView > CTreeItemView:active {
	background: rgb(6, 66, 126);
}

#TopBarLayout {
	padding: 10px 3px;
	height: 40px;
	flex-direction: row;
	justify-content: flex-start;
	align-items: center;
	column-gap: 15px;
}

#SearchBox {
	width: 160px;
	height: 26px;
}

#AssetGridView {
	width: 100%;
	flex-grow: 1;
	flex-shrink: 3;
	flex-direction: row;
	flex-wrap: wrap;
	align-items: flex-start;
	align-self: stretch;
	padding: 10px 10px;
	row-gap: 20px;
	column-gap: 20px;
}

#AddButton::icon {
    background-image: url(/Icons/plus_green.png);
}


)";

namespace CE::Editor
{

	AssetBrowserPanel::AssetBrowserPanel()
	{
		SetAllowHorizontalScroll(false);
		SetAllowVerticalScroll(false);

		SetTitle("Asset Browser");
	}

	AssetBrowserPanel::~AssetBrowserPanel()
	{
		
	}

	void AssetBrowserPanel::Construct()
	{
		Super::Construct();

		SetStyleSheet(css);
		
		left = CreateWidget<CContainerWidget>(this, "LeftContainer");
		left->SetHorizontalScrollAllowed(true);
		left->SetVerticalScrollAllowed(true);
		{
			auto gameContentSection = CreateWidget<CCollapsibleSection>(left, "GameContentSection");
			gameContentSection->SetTitle("Game Content");
			gameContentSection->SetCollapsed(false);
			
			gameContentDirectoryView = CreateWidget<CTreeView>(gameContentSection, "GameContentTreeView");
			auto model = CreateObject<AssetBrowserTreeModel>(gameContentDirectoryView, "GameContentTreeModel");
			model->SetPathTreeRootNode(AssetManager::GetRegistry()->GetCachedPathTree().GetNode("/Game"));
			gameContentDirectoryView->SetModel(model);
			gameContentDirectoryView->SetItemsSelectable(true);

			Object::Bind(model, MEMBER_FUNCTION(AssetBrowserTreeModel, OnSelectionChanged), 
				this, MEMBER_FUNCTION(Self, OnGameContentTreeViewSelectionChanged));

			auto engineContentSection = CreateWidget<CCollapsibleSection>(left, "EngineContentSection");
			engineContentSection->SetTitle("Engine Content");
		}

		right = CreateWidget<CContainerWidget>(this, "RightContainer");
		right->SetVerticalScrollAllowed(false);
		{
			auto topBarLayout = CreateWidget<CLayoutGroup>(right, "TopBarLayout");
			{
				auto addButton = CreateWidget<CButton>(topBarLayout, "AddButton");
				addButton->SetText("Add");
				//addButton->LoadIcon("/Icons/plus_green.png");

				auto searchBox = CreateWidget<CTextInput>(topBarLayout, "SearchBox");
				searchBox->SetHint("Search...");

				CreateWidget<CSpacer>(topBarLayout, "Spacer");

				auto settingsButton = CreateWidget<CButton>(topBarLayout, "SettingsButton");
				settingsButton->AddStyleClass("Transparent");
				settingsButton->SetText("Settings");

				auto settingsButtonMenu = CreateWidget<CMenu>(settingsButton, "SettingsButtonMenu");
				settingsButtonMenu->SetMenuPosition(CMenuPosition::Bottom);
				settingsButton->SetPopupMenu(settingsButtonMenu);
				{
					for (int i = 0; i < 8; i++)
					{
						if (i == 4)
						{
							auto header = CreateWidget<CMenuItemHeader>(settingsButtonMenu, "Header");
							header->SetTitle("TITLE");
						}

						auto menuItem = CreateWidget<CMenuItem>(settingsButtonMenu, "MenuItem");
						menuItem->SetText(String::Format("Item No. {}", i));
						menuItem->LoadIcon("/Icons/folder_32.png");

						if (i == 3)
						{
							auto subMenu = CreateWidget<CMenu>(menuItem, "SubMenu");
							menuItem->SetSubMenu(subMenu);
							
							for (int j = 0; j < 4; j++)
							{
								auto subMenuItem = CreateWidget<CMenuItem>(subMenu, "SubMenuItem");
								subMenuItem->SetText(String::Format("Sub Item {}", j));
							}
						}
					}
				}
			}

			CreateWidget<CSeparator>(right, "Separator");

			auto gridView = CreateWidget<CContainerWidget>(right, "AssetGridView");
			gridView->SetHorizontalScrollAllowed(false);
			gridView->SetVerticalScrollAllowed(true);
			{
				for (int i = 0; i < 16; i++)
				{
					auto item = CreateWidget<AssetItemWidget>(gridView, "AssetItem");
				}
			}
		}
	}

	void AssetBrowserPanel::OnGameContentTreeViewSelectionChanged(PathTreeNode* selectedNode)
	{

	}

}

