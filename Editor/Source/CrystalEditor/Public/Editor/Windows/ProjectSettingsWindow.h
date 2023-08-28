#pragma once

namespace CE::Editor
{
    CLASS()
	class CRYSTALEDITOR_API ProjectSettingsWindow : public EditorPanel
	{
		CE_CLASS(ProjectSettingsWindow, EditorPanel)
	public:

		ProjectSettingsWindow();
		virtual ~ProjectSettingsWindow();

	protected:

		void Construct() override;

		ObjectEditor* editor = nullptr;
	};

} // namespace CE::Editor

#include "ProjectSettingsWindow.rtti.h"
