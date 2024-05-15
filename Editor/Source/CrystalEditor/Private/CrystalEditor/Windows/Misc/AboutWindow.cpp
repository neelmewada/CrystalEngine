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

	void AboutWindow::OnPlatformWindowSet()
	{
		Super::OnPlatformWindowSet();

		if (nativeWindow)
		{
			nativeWindow->SetAlwaysOnTop(true);
		}
	}

	void AboutWindow::Construct()
	{
		Super::Construct();

		LoadStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/AboutWindowStyle.css");

		CWidget* splash = CreateObject<CWidget>(this, "Splash");

		CLabel* titleLabel = CreateObject<CLabel>(this, "TitleLabel");
		titleLabel->SetText("CrystalEditor");

		CLabel* desc1 = CreateObject<CLabel>(this, "Desc1");
		desc1->SetText("Version: " CE_ENGINE_VERSION_STRING_SHORT " pre-alpha [Build " CE_TOSTRING(CE_VERSION_BUILD) "]");

		CLabel* desc2 = CreateObject<CLabel>(this, "Desc2");
		desc2->SetText("Platform: " + PlatformMisc::GetOSVersionString());

		CLabel* attributionLabel = CreateObject<CLabel>(this, "AttributionLabel");
		attributionLabel->SetText("Icons by flaticon.com");
	}

} // namespace CE::Editor
