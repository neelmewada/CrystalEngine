#include "CrystalEditor.h"

static const CE::String css = R"(
#HorizontalLayout {
	margin: 0 5px 0 0;
	flex-direction: row;
	align-items: center;
}
)";


namespace CE::Editor
{
	static CrystalEditorWindow* gEditorWindow = nullptr;

	CrystalEditorWindow::CrystalEditorWindow()
	{
		
	}

	CrystalEditorWindow::~CrystalEditorWindow()
	{
		gEditorWindow = nullptr;
	}

	CrystalEditorWindow* CrystalEditorWindow::Get()
	{
		return gEditorWindow;
	}

	void CrystalEditorWindow::Construct()
	{
		gEditorWindow = this;
		
		LoadStyleSheet("/CrystalEditor/Resources/Styles/CrystalEditorWindow.css");

		SetAsDockSpaceWindow(true);
		SetTitle("Crystal Editor");
		SetFullscreen(true);

		Super::Construct();

		auto yesNoPopup = CreateWidget<CPopup>(this, "YesNoPopup");
		yesNoPopup->SetTitle("Demo Popup");
		yesNoPopup->SetStyleSheet(css);
		{
			auto text = CreateWidget<CLabel>(yesNoPopup, "TextArea");
			text->SetText("This is a long form text. And this is the second sentence. Please click yes or no button below.");

			auto horizontalGroup = CreateWidget<CLayoutGroup>(yesNoPopup, "HorizontalLayout");

			CreateWidget<CSpacer>(horizontalGroup, "Spacer");

			auto yesBtn = CreateWidget<CButton>(horizontalGroup, "YesButton");
			yesBtn->SetAsAlternateStyle(true);
			yesBtn->SetText("Yes");

			auto noBtn = CreateWidget<CButton>(horizontalGroup, "NoButton");
			noBtn->SetText("No");

			Object::Bind(yesBtn, MEMBER_FUNCTION(CButton, OnButtonClicked), [=]
				{
					yesNoPopup->Hide();
				});

			Object::Bind(noBtn, MEMBER_FUNCTION(CButton, OnButtonClicked), [=]
				{
					yesNoPopup->Hide();
				});
		}
		assetImportPopup = yesNoPopup;

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

	void CrystalEditorWindow::ShowDemoPopup()
	{
		assetImportPopup->Show();
	}

} // namespace CE::Editor
