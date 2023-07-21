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
	padding: 10px 5px 23px 5px;
	background: rgb(26, 26, 26);
	flex-direction: column;
	justify-content: flex-start;
}

#RightContainer {
	width: 70%;
	height: 100%;
}

#GameContentSection {
	
}

#GameContentTreeView {
	padding: 0px 10px;
}

#GameContentTreeView > CTreeItemView {
	text-align: middle-left;
	font-size: 18px;
	height: 20px;
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

			gameContentDirectoryView = CreateWidget<CTreeView>(gameContentSection, "GameContentTreeView");
			auto model = CreateObject<AssetBrowserTreeModel>(gameContentDirectoryView, "GameContentTreeModel");
			model->SetPathTreeRootNode(AssetManager::GetRegistry()->GetCachedPathTree().GetNode("/Game"));
			gameContentDirectoryView->SetModel(model);

			auto engineContentSection = CreateWidget<CCollapsibleSection>(left, "EngineContentSection");
			engineContentSection->SetTitle("Engine Content");
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

