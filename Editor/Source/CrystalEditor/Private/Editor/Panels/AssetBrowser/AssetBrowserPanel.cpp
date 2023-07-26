#include "CrystalEditor.h"

static const CE::String css = R"(

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

		//SetStyleSheet(css);
        LoadStyleSheet("/CrystalEditor/Resources/Styles/AssetBrowserPanel.css");
		
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

