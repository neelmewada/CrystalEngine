#include "CrystalEditor.h"

namespace CE::Editor
{

	AboutWindow::AboutWindow()
	{
		title = "About Crystal Editor";

		canBeClosed = true;
		canBeMaximized = canBeMinimized = false;
	}

	AboutWindow::~AboutWindow()
	{
		
	}

	void AboutWindow::Construct()
	{
		Super::Construct();

		LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/AboutWindowStyle.css");

		CWidget* splash = CreateObject<CWidget>(this, "Splash");

		CLabel* titleLabel = CreateObject<CLabel>(this, "TitleLabel");
		titleLabel->SetText("CrystalEditor");

		CLabel* desc1 = CreateObject<CLabel>(this, "Desc1");
		desc1->SetText("Crystal Engine Version: " CE_ENGINE_VERSION_STRING);

		CLabel* desc2 = CreateObject<CLabel>(this, "Desc2");
		desc2->SetText("Platform: " + PlatformMisc::GetPlatformName());

		CLabel* attribution = CreateObject<CLabel>(this, "AttributionLinks");
		attribution->SetText("Images by freepik.com");
	}

} // namespace CE::Editor
