#include "CrystalEditor.h"

static const CE::String css = R"(


)";

/*
AssetBrowserView {
	flex-direction: row;
	align-items: stretch;
	height: 100%;
}

#LeftContainer {
	width: 25%;
	height: 100%;
}

#RightContainer {
	width: 75%;
	height: 100%;
}
*/

namespace CE::Editor
{

	AssetBrowserView::AssetBrowserView()
	{
		
	}

	AssetBrowserView::~AssetBrowserView()
	{

	}

	void AssetBrowserView::Construct()
	{
		Super::Construct();

		left = CreateWidget<CContainerWidget>(this, "LeftContainer");

		auto gameAssetsSection = CreateWidget<CCollapsibleSection>(left, "GameAssetsSection");
		gameAssetsSection->SetTitle("Game Assets");
		{
			auto label = CreateWidget<CLabel>(gameAssetsSection, "TestLabel");
			label->SetText("This is a sub-label");

			auto btn = CreateWidget<CButton>(gameAssetsSection, "TestButton");
			btn->SetText("Button");
		}
		
		right = CreateWidget<CContainerWidget>(this, "RightContainer");
		{
			auto label = CreateWidget<CLabel>(right, "TestLabel");
			label->SetText("This is a sub-label");

			auto btn = CreateWidget<CButton>(right, "TestButton");
			btn->SetText("Button");
		}
	}

}

