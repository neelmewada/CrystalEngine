#include "CrystalEditor.h"

namespace CE::Editor
{
	CrystalEditorWindow::CrystalEditorWindow()
	{
		
	}

	CrystalEditorWindow::~CrystalEditorWindow()
	{

	}

	void CrystalEditorWindow::Construct()
	{
		SetAsDockSpaceWindow(true);
		SetTitle("Crystal Editor");
		SetFullscreen(true);

		Super::Construct();

		auto menuBar = CreateWidget<CMenuBar>(this, "EditorMenuBar");
		{
			auto fileMenuItem = CreateWidget<CMenuItem>(menuBar, "FileMenuItem");
			fileMenuItem->SetText("File");
			{
				auto fileMenu = CreateWidget<CMenu>(fileMenuItem, "FileMenu");
				fileMenuItem->SetSubMenu(fileMenu);

				for (int i = 0; i < 8; i++)
				{
					auto item = CreateWidget<CMenuItem>(fileMenu, "FileMenuSubItem");
					item->SetText(String::Format("File Item {}", i));

					if (i == 3)
					{
						auto subMenu = CreateWidget<CMenu>(item, "SubMenu");
						item->SetSubMenu(subMenu);
						for (int j = 0; j < 4; j++)
						{
							auto subItem = CreateWidget<CMenuItem>(subMenu, "SubItem");
							subItem->SetText(String::Format("Sub Item: {}", j));

							if (j == 1)
								subItem->SetAsToggle();
						}
					}
				}
			}

			auto editMenuItem = CreateWidget<CMenuItem>(menuBar, "EditMenuItem");
			editMenuItem->SetText("Edit");
			{
				auto editMenu = CreateWidget<CMenu>(editMenuItem, "EditMenu");
				editMenuItem->SetSubMenu(editMenu);

				for (int i = 0; i < 8; i++)
				{
					auto item = CreateWidget<CMenuItem>(editMenu, "EditMenuSubItem");
					item->SetText(String::Format("Edit Item {}", i));
				}
			}
		}

		assetBrowserPanel = CreateWidget<AssetBrowserPanel>(this, "AssetBrowserPanel");
	}

} // namespace CE::Editor
