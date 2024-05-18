#pragma once

namespace CE::Editor
{

	CLASS()
	class CRYSTALEDITOR_API ProjectSettingsWindow final : public CToolWindow
	{
		CE_CLASS(ProjectSettingsWindow, CToolWindow)
	public:

		ProjectSettingsWindow();

		virtual ~ProjectSettingsWindow();

	private:

		void OnPlatformWindowSet() override;

		void Construct() override;

		FIELD(ReadOnly)
		CSplitView* splitView = nullptr;

		FIELD(ReadOnly)
		CSplitViewContainer* sideBar = nullptr;

		FIELD(ReadOnly)
		CSplitViewContainer* container = nullptr;

	};

} // namespace CE::Editor

#include "ProjectSettingsWindow.rtti.h"