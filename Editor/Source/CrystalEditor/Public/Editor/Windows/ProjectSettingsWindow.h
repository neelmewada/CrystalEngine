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

		void ConstructEditor();

		FIELD()
		CSplitView* splitView = nullptr;

		FIELD()
		CLayoutGroup* rightView = nullptr;

		FIELD()
		CLabel* editorTitleLabel = nullptr;
		
		FIELD(ReadOnly)
		ObjectEditor* editor = nullptr;

		FIELD(ReadOnly)
		SettingsBase* target = nullptr;
	};

} // namespace CE::Editor

#include "ProjectSettingsWindow.rtti.h"
