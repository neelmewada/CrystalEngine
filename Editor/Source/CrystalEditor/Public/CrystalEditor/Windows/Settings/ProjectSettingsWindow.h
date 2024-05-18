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
		CWidget* sideBar = nullptr;

		FIELD(ReadOnly)
		CWidget* content = nullptr;

	};

} // namespace CE::Editor

#include "ProjectSettingsWindow.rtti.h"