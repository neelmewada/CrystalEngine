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

        LoadStyleSheet("/CrystalEditor/Resources/Styles/AssetBrowserPanel.css");
		
		left = CreateWidget<CContainerWidget>(this, "LeftContainer");
		left->SetHorizontalScrollAllowed(true);
		left->SetVerticalScrollAllowed(true);
		{
			auto gameContentSection = CreateWidget<CCollapsibleSection>(left, "GameContentSection");
			gameContentSection->SetTitle("Game Content");
			gameContentSection->SetCollapsed(false);
			
			gameContentDirectoryView = CreateWidget<CTreeView>(gameContentSection, "GameContentTreeView");
			folderModel = CreateObject<AssetBrowserTreeModel>(gameContentDirectoryView, "GameContentTreeModel");
			folderModel->SetPathTreeRootNode(AssetManager::GetRegistry()->GetCachedDirectoryPathTree().GetNode("/Game"));
			gameContentDirectoryView->SetModel(folderModel);
			gameContentDirectoryView->SetItemsSelectable(true);

			Object::Bind(folderModel, MEMBER_FUNCTION(AssetBrowserTreeModel, OnSelectionChanged),
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

			assetGridView = CreateWidget<CContainerWidget>(right, "AssetGridView");
			assetGridView->SetInteractable(true);
			assetGridView->SetHorizontalScrollAllowed(false);
			assetGridView->SetVerticalScrollAllowed(true);
			if (false)
			{
				for (int i = 0; i < 16; i++)
				{
					auto item = CreateWidget<AssetItemWidget>(assetGridView, "AssetItem");
					item->SetAsFolder();
					item->SetLabel(String::Format("This is a long folder name {}.", i));
					assetItems.Add(item);

					Object::Bind(item, MEMBER_FUNCTION(AssetItemWidget, OnButtonClicked), [=]
						{
							SetSelectedItem(i);
						});

					Object::Bind(item, MEMBER_FUNCTION(AssetItemWidget, OnItemDoubleClicked), [=]
						{
							
						});
				}
			}
		}


		// Select root game assets directory
		gameContentDirectoryView->Select(folderModel->GetIndex(0, 0, {}));
	}

	void AssetBrowserPanel::SetCurrentAssetDirectory(const Name& path)
	{
		auto directoryNode = AssetManager::GetRegistry()->GetCachedDirectoryPathTree().GetNode(path);
		if (directoryNode == nullptr)
		{
			CE_LOG(Error, All, "Invalid asset directory: {}", path);
			return;
		}

		if (directoryNode->parent == nullptr)
		{
			gameContentDirectoryView->Select(folderModel->GetIndex(0, 0, {}));
			return;
		}

		CModelIndex indexToSelect = folderModel->FindIndex(directoryNode);
		if (!indexToSelect.IsValid())
		{
			return;
		}


	}

	void AssetBrowserPanel::UpdateContentView()
	{
		if (!selectedDirectoryPath.IsValid())
			return;

		PathTreeNode* contentDirectoryNode = AssetManager::GetRegistry()->GetCachedPathTree().GetNode(selectedDirectoryPath);
		if (contentDirectoryNode == nullptr)
			return;

		int newCount = contentDirectoryNode->children.GetSize();
		auto curCount = assetGridView->GetSubWidgetCount();

		//if (curCount > newCount) // Remove extra widgets
		{
			for (int i = 0; i < curCount; i++)
			{
				auto freeWidget = (AssetItemWidget*)assetGridView->GetSubWidgetAt(assetGridView->GetSubWidgetCount() - 1);
				assetGridView->RemoveSubWidget(freeWidget);
				freeAssetItems.Add(freeWidget);
			}
		}

		assetItems.Clear();

		for (int i = 0; i < newCount; i++)
		{
			PathTreeNode* node = contentDirectoryNode->children[i];

			AssetItemWidget* widget = nullptr;
			if (freeAssetItems.NonEmpty())
			{
				widget = freeAssetItems.Top();
				freeAssetItems.Pop();
				assetGridView->AddSubWidget(widget);
			}
			else
			{
				widget = CreateWidget<AssetItemWidget>(assetGridView, "AssetItem");

				Object::Bind(widget, MEMBER_FUNCTION(AssetItemWidget, OnButtonClicked), [=]
					{
						SetSelectedItem(widget);
					});

				Object::Bind(widget, MEMBER_FUNCTION(AssetItemWidget, OnItemDoubleClicked), [=]
					{

					});
			}

			if (node->nodeType == PathTreeNodeType::Directory)
				widget->SetAsFolder();
			else
				widget->SetAsAsset();

			IO::Path nameWithoutExtension = node->name.GetString();
			if (node->nodeType == PathTreeNodeType::Asset)
				nameWithoutExtension = nameWithoutExtension.RemoveExtension();

			widget->SetLabel(nameWithoutExtension.GetString());
			widget->SetPath(node->GetFullPath());

			assetItems.Add(widget);
		}

		SetSelectedItem(nullptr);
	}

	void AssetBrowserPanel::HandleEvent(CEvent* event)
	{
		if (event->type == CEventType::MouseButtonClick)
		{
			bool isHandled = event->isHandled;
			CMouseEvent* mouseEvent = (CMouseEvent*)event;
			if (mouseEvent->mouseButton == 0 && event->sender == assetGridView)
			{
				SetSelectedItem(nullptr);
				event->HandleAndStopPropagation();
			}
		}

		Super::HandleEvent(event);
	}

	void AssetBrowserPanel::SetSelectedItem(int index)
	{
		for (int i = 0; i < assetItems.GetSize(); i++)
		{
			if (index == i)
				assetItems[i]->AddStyleClass("Selected");
			else
				assetItems[i]->RemoveStyleClass("Selected");
		}
	}

	void AssetBrowserPanel::SetSelectedItem(AssetItemWidget* item)
	{
		for (int i = 0; i < assetItems.GetSize(); i++)
		{
			if (item == assetItems[i])
				assetItems[i]->AddStyleClass("Selected");
			else
				assetItems[i]->RemoveStyleClass("Selected");
		}
	}

	void AssetBrowserPanel::OnGameContentTreeViewSelectionChanged(PathTreeNode* selectedNode)
	{
		if (selectedNode == nullptr)
			return;

		selectedDirectoryPath = selectedNode->GetFullPath();
		UpdateContentView();
	}

	void AssetBrowserPanel::OnEngineContentTreeViewSelectionChanged(PathTreeNode* selectedNode)
	{

	}

}

