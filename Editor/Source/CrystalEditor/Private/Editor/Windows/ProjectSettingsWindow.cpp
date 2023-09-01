#include "CrystalEditor.h"

namespace CE::Editor
{
	ProjectSettingsWindow::ProjectSettingsWindow()
	{

	}

	ProjectSettingsWindow::~ProjectSettingsWindow()
	{

	}

	void ProjectSettingsWindow::Construct()
	{
		Super::Construct();

		SetTitle("Project Settings");
		SetFullscreen(false);

		splitView = CreateWidget<CSplitView>(this, "ProjectSettingsSplitView");
		splitView->SetInitialSplit(0.25f);
		splitView->SetStretchToFill(true);

		auto leftView = CreateWidget<CLayoutGroup>(splitView->GetLeft(), "ProjectSettingsLeftView");

		auto rightView = CreateWidget<CLayoutGroup>(splitView->GetRight(), "ProjectSettingsRightView");

		auto titleLabel  = CreateWidget<CLabel>(rightView, "TitleLabel");
		titleLabel->SetText("Project");
		titleLabel->AddStyleClass("Title");

		editor = CreateWidget<ObjectEditor>(rightView, "ProjectSettingsObjectEditor");
		editor->SetTargets({ GetSettings<ProjectSettings>() });
	}

} // namespace CE::Editor
