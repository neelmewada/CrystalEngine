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
		return Vec2(width, YGUndefined);
	}

	void SceneOutlinerTree::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();

		auto tableFlags = GUI::TableFlags_Resizable | GUI::TableFlags_RowBg | GUI::TableFlags_ScrollY;

		GUI::PushStyleColor(GUI::StyleCol_TableRowBg, Color::RGBA(21, 21, 21));
		GUI::PushStyleColor(GUI::StyleCol_TableRowBgAlt, Color::RGBA(26, 26, 26));

		if (GUI::BeginTable(rect + Rect(5, 0, 0, -22), GetUuid(), "", 2, tableFlags))
		{
			GUI::TableSetupColumn("Node Label");
			GUI::TableSetupColumn("Type");
			GUI::TableHeadersRow();

			for (int i = 0; i < 32; i++)
			{
				GUI::TableNextRow();

				GUI::TableNextColumn();
				GUI::Selectable(String::Format("Node {}", i), false, GUI::SelectableFlags_SpanAllColumns);

				GUI::TableNextColumn();
				GUI::Text("Node");
			}

			GUI::EndTable();
		}

		GUI::PopStyleColor(2);
	}

    SceneOutlinerPanel::SceneOutlinerPanel()
    {

    }

    SceneOutlinerPanel::~SceneOutlinerPanel()
    {

    }

	void SceneOutlinerPanel::SetScene(Scene* scene)
	{
		this->scene = scene;


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

		tree = CreateWidget<SceneOutlinerTree>(this, "SceneOutlinerTree");
		{

		}
    }

	void SceneOutlinerPanel::OnDrawGUI()
	{
		Super::OnDrawGUI();
	}

} // namespace CE::Editor
