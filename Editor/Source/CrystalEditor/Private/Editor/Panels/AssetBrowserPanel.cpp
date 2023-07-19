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

			gameContentDirectoryView = CreateWidget<CTableView>(gameContentSection, "GameContentTreeView");
			auto model = CreateObject<AssetBrowserTreeModel>(gameContentDirectoryView, "GameContentTreeModel");
			model->SetPathTreeRootNode(AssetManager::GetRegistry()->GetCachedPathTree().GetNode("/Game"));
			gameContentDirectoryView->SetModel(model);
		}

		right = CreateWidget<CContainerWidget>(this, "RightContainer");
		right->SetVerticalScrollAllowed(false);
		{
			auto label = CreateWidget<CLabel>(right, "Lbl");
			label->SetText("This is a very long label. It has 2 sentences.");

			auto btn = CreateWidget<CButton>(right, "Btn");
			btn->SetText("Button");
		}
	}

}

