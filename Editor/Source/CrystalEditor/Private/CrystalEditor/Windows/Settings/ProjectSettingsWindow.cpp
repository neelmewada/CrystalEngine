#include "CrystalEditor.h"

namespace CE::Editor
{

	ProjectSettingsWindow::ProjectSettingsWindow()
	{
		title = "Project Settings";

		canBeClosed = true;
		canBeMaximized = canBeMinimized = true;
	}

	ProjectSettingsWindow::~ProjectSettingsWindow()
	{
		
	}

	void ProjectSettingsWindow::OnPlatformWindowSet()
	{
		Super::OnPlatformWindowSet();

		if (nativeWindow)
		{
			nativeWindow->SetAlwaysOnTop(true);
			nativeWindow->platformWindow->SetResizable(true);
		}
	}

	void ProjectSettingsWindow::Construct()
	{
		Super::Construct();

		LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/ProjectSettingsStyle.css");

		splitView = CreateObject<CSplitView>(this, "ProjectSettingsSplitView");
		splitView->SetOrientation(COrientation::Horizontal);

		splitView->AddSplit(0.75f);

		sideBar = splitView->GetContainer(0);
		sideBar->SetVerticalScrollAllowed(true);
		sideBar->AddBehavior<CScrollBehavior>();

		container = splitView->GetContainer(1);
		container->SetVerticalScrollAllowed(true);
		container->AddBehavior<CScrollBehavior>();


	}

} // namespace CE::Editor
