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

		void ShowSettingsFor(SubClass<Settings> settingsClass);

	private:

		void OnPlatformWindowSet() override;

		void Construct() override;

		void OnPaint(CPaintEvent* paintEvent) override;

		FIELD(ReadOnly)
		CWidget* sideBar = nullptr;

		FIELD(ReadOnly)
		CWidget* content = nullptr;

		FIELD(ReadOnly)
		ObjectEditor* editor = nullptr;

	};

} // namespace CE::Editor

#include "ProjectSettingsWindow.rtti.h"