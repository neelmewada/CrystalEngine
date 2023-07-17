#include "CrystalEditor.h"

static const CE::String css = R"(

AssetBrowserPanel {
	padding: 0 25px 0 0;
	position: absolute;
	top: 0; left: 0; bottom: 0; right: 0;
	flex-direction: row;
}

CContainerWidget {
	padding: 10px 5px;
}

#LeftContainer {
	width: 30%;
	height: 100%;
	background: rgb(26, 26, 26);
}

#RightContainer {
	width: 70%;
	height: 100%;
	background: red;
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

			auto label = CreateWidget<CLabel>(gameContentSection, "Lbl");
			label->SetText("This is a very long label. It has 2 sentences.");

			auto btn = CreateWidget<CButton>(gameContentSection, "Btn");
			btn->SetText("Button");
		}

		right = CreateWidget<CContainerWidget>(this, "RightContainer");
		right->SetVerticalScrollAllowed(false);
		{

		}
	}

}

