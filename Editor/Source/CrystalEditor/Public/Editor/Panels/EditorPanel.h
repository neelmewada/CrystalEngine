#pragma once

namespace CE::Editor
{
    CLASS()
	class CRYSTALEDITOR_API EditorPanel : public CWindow
	{
		CE_CLASS(EditorPanel, CWindow)
	public:

		EditorPanel();
		virtual ~EditorPanel();

		void Construct() override;

		bool IsSubWidgetAllowed(ClassType* subwidgetClass) override;

	protected:

		void OnDrawGUI() override;

	};

} // namespace CE::Editor

#include "EditorPanel.rtti.h"
