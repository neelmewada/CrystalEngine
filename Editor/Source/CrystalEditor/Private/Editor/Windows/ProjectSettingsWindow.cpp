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

		editor = CreateWidget<ObjectEditor>(this, "ObjectEditor");
		editor->SetTargets({ GetSettings<ProjectSettings>() });
		
	}

} // namespace CE::Editor
