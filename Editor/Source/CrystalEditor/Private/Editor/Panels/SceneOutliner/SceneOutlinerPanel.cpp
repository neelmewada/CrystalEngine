#include "CrystalEditor.h"

namespace CE::Editor
{
	SceneOutlinerTree::SceneOutlinerTree()
	{

	}

	SceneOutlinerTree::~SceneOutlinerTree()
	{

	}

	Vec2 SceneOutlinerTree::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		if (height <= 0)
			height = YGUndefined;
		return Vec2(width, height);
	}

	void SceneOutlinerTree::OnDrawGUI()
	{

	}

    SceneOutlinerPanel::SceneOutlinerPanel()
    {

    }

    SceneOutlinerPanel::~SceneOutlinerPanel()
    {

    }

    void SceneOutlinerPanel::Construct()
    {
		Super::Construct();

		SetTitle("Scene Outliner");

		LoadStyleSheet("/CrystalEditor/Resources/Styles/SceneOutlinerPanel.css");

		auto topBar = CreateWidget<CLayoutGroup>(this, "SceneOutlinerTopBar");
		topBar->AddStyleClass("Horizontal");
		{
			auto searchBox = CreateWidget<CTextInput>(topBar, "SceneOutlinerSearchBox");
			searchBox->SetHint("Search...");

			auto settingsBtn = CreateWidget<CButton>(topBar, "SceneOutlinerSettingsButton");
			settingsBtn->SetText("");
			settingsBtn->SetIconSize(18);
			settingsBtn->AddStyleClass("IconButton");
			settingsBtn->LoadIcon("/Icons/settings.png");
		}
    }

	void SceneOutlinerPanel::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}

} // namespace CE::Editor
