#include "CrystalEditor.h"

static const CE::String css = R"(

)";

namespace CE::Editor
{
	String Private::AssetBrowserPanelPrefs::PrefsKey()
	{
		return TYPENAME(AssetBrowserPanel).GetString() + ".Prefs";
	}

	static const Array<String> thumbnailSizes = { "Small", "Medium", "Large", "Huge" };

	AssetBrowserPanel::AssetBrowserPanel()
	{
		SetAllowHorizontalScroll(false);
		SetAllowVerticalScroll(false);

		defaultDockPosition = CDockPosition::Bottom;
	}

	AssetBrowserPanel::~AssetBrowserPanel()
	{
		if (assetRegistryModified != 0 && AssetRegistry::Get() != nullptr)
		{
			AssetRegistry::Get()->onAssetRegistryModified.RemoveDelegateInstance(assetRegistryModified);
		}
	}

	void AssetBrowserPanel::Construct()
	{
		Super::Construct();

		EditorPrefs::Get().GetStruct(Private::AssetBrowserPanelPrefs::PrefsKey(), &prefs);
		SetTitle("Asset Browser");

        LoadStyleSheet("/CrystalEditor/Resources/Styles/AssetBrowserPanel.css");
		
		left = CreateWidget<CContainerWidget>(this, "LeftContainer");
		left->SetHorizontalScrollAllowed(true);
		left->SetVerticalScrollAllowed(true);
		{
			//auto gameContentSection = CreateWidget<CCollapsibleSection>(left, "GameContentSection");
			//gameContentSection->SetTitle("Content");
			//gameContentSection->SetCollapsed(false);
			
			gameContentDirectoryView = CreateWidget<CTreeView>(left, "GameContentTreeView");
			folderModel = CreateObject<AssetBrowserTreeModel>(gameContentDirectoryView, "GameContentTreeModel");
			folderModel->SetPathTreeRootNode(AssetManager::GetRegistry()->GetCachedDirectoryPathTree().GetNode("/Game"));
			gameContentDirectoryView->SetModel(folderModel);
			gameContentDirectoryView->SetItemsSelectable(true);

			Object::Bind(folderModel, MEMBER_FUNCTION(AssetBrowserTreeModel, OnSelectionChanged),
				this, MEMBER_FUNCTION(Self, OnGameContentTreeViewSelectionChanged));
		}

		right = CreateWidget<CContainerWidget>(this, "RightContainer");
		right->SetVerticalScrollAllowed(false);
		{
			auto topBarLayout = CreateWidget<CLayoutGroup>(right, "TopBarLayout");
			{
				auto addButton = CreateWidget<CButton>(topBarLayout, "AddButton");
				addButton->SetText("Add");
				Object::Bind(addButton, MEMBER_FUNCTION(CButton, OnButtonClicked), []
					{
						
					});
				
				auto searchBox = CreateWidget<CTextInput>(topBarLayout, "SearchBox");
				searchBox->SetHint("Search...");

				CreateWidget<CSpacer>(topBarLayout, "Spacer");

				auto settingsButton = CreateWidget<CButton>(topBarLayout, "SettingsButton");
				settingsButton->AddStyleClass("Transparent");
				settingsButton->SetText("Settings");

				auto settingsButtonMenu = CreateWidget<CMenu>(settingsButton, "SettingsButtonMenu");
				settingsButtonMenu->SetMenuPosition(CMenuPosition::Top);
				settingsButton->SetPopupMenu(settingsButtonMenu);
				{
					auto header = CreateWidget<CMenuItemHeader>(settingsButtonMenu, "Header");
					header->SetTitle("VIEW");
					
					auto thumbnailSizeMenuItem = CreateWidget<CMenuItem>(settingsButtonMenu, "ThumbnailSizeMenuItem");
					thumbnailSizeMenuItem->SetText("Thumbnail Size");
					auto thumbnailSizeSubMenu = CreateWidget<CMenu>(thumbnailSizeMenuItem, "ThumbnailSizeSubMenu");
					thumbnailSizeMenuItem->SetSubMenu(thumbnailSizeSubMenu);
					{
						CMenuItem* radioToEnable = nullptr;
						if (prefs.thumbnailSize < 0 || prefs.thumbnailSize >= thumbnailSizes.GetSize())
							prefs.thumbnailSize = 1;

						for (int i = 0; i < thumbnailSizes.GetSize(); i++)
						{
							auto value = CreateWidget<CMenuItem>(thumbnailSizeSubMenu, "ThumbnailSizeRadio");
							value->SetText(thumbnailSizes[i]);
							value->SetAsRadio("ThumbnailSizeValue");
							Object::Bind(value, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), 
								this, MEMBER_FUNCTION(Self, OnThumbnailSizeMenuItemClicked));

							if (i == prefs.thumbnailSize)
							{
								radioToEnable = value;
							}
						}

						if (radioToEnable != nullptr)
							radioToEnable->SetRadioValue(true);
					}
				}
			}

			CreateWidget<CSeparator>(right, "Separator");

			assetGridView = CreateWidget<CContainerWidget>(right, "AssetGridView");
			assetGridView->SetInteractable(true);
			assetGridView->SetHorizontalScrollAllowed(false);
			assetGridView->SetVerticalScrollAllowed(true);
			assetGridView->AddStyleClass(thumbnailSizes[prefs.thumbnailSize]);
		}


		// Select root game assets directory
		gameContentDirectoryView->Select(folderModel->GetIndex(0, 0, {}));

		assetRegistryModified = AssetRegistry::Get()->onAssetRegistryModified.AddDelegateInstance(MemberDelegate(&Self::UpdateContentView, this));
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

		auto indexInParent = directoryNode->parent->children.IndexOf(directoryNode);
		
		CModelIndex indexToSelect = folderModel->CreateIndex(indexInParent, 0, directoryNode);
		if (!indexToSelect.IsValid())
		{
			return;
		}

		if (path.GetString().StartsWith("/Game") || path.GetString().StartsWith("Game"))
			gameContentDirectoryView->Select(indexToSelect);
	}

	void AssetBrowserPanel::UpdateContentView()
	{
		if (!selectedDirectoryPath.IsValid())
			return;

		AssetRegistry* registry = AssetRegistry::Get();
		PathTreeNode* contentDirectoryNode = registry->GetDirectoryNode(selectedDirectoryPath);
		if (contentDirectoryNode == nullptr)
			return;

		int newCount = contentDirectoryNode->children.GetSize();
		auto curCount = assetGridView->GetSubWidgetCount();

		// Remove all sub-widgets
		{
			for (int i = 0; i < curCount; i++)
			{
				auto freeWidget = (AssetItemWidget*)assetGridView->GetSubWidgetAt(assetGridView->GetSubWidgetCount() - 1);
				assetGridView->RemoveSubWidget(freeWidget);
				freeAssetItems.Add(freeWidget);
			}
		}

		assetItems.Clear();

		// Add directories
		for (int i = 0; i < newCount; i++)
		{
			PathTreeNode* node = contentDirectoryNode->children[i];
			if (node->nodeType != PathTreeNodeType::Directory)
				continue;

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
						if (widget->IsAssetItem())
							HandleOpenAsset(widget);
						else
							SetCurrentAssetDirectory(widget->GetPath());
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

		// Add Assets
		auto subAssets = registry->GetPrimaryAssetsInSubPath(selectedDirectoryPath);

		for (int i = 0; i < subAssets.GetSize(); i++)
		{
			auto subAsset = subAssets[i];

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
						if (widget->IsAssetItem())
							HandleOpenAsset(widget);
						else
							SetCurrentAssetDirectory(widget->GetPath());
					});
			}

			widget->SetAsAsset();

			String name = subAsset->packageName.GetLastComponent();

			widget->SetLabel(name);

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

	void AssetBrowserPanel::HandleOpenAsset(AssetItemWidget* item)
	{
		
	}

	void AssetBrowserPanel::OnThumbnailSizeMenuItemClicked(CMenuItem* menuItem)
	{
		if (menuItem == nullptr)
			return;

		for (const auto& size : thumbnailSizes)
		{
			assetGridView->RemoveStyleClass(size);
		}

		for (int i = 0; i < thumbnailSizes.GetSize(); i++)
		{
			if (thumbnailSizes[i] == menuItem->GetText())
			{
				assetGridView->AddStyleClass(thumbnailSizes[i]);

				prefs.thumbnailSize = i;
				EditorPrefs::Get().SetStruct(Private::AssetBrowserPanelPrefs::PrefsKey(), &prefs);
				break;
			}
		}

		assetGridView->SetNeedsStyle();
		assetGridView->SetNeedsLayout();
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

