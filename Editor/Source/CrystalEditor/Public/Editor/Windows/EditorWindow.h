#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API EditorWindow : public CWindow
	{
		CE_CLASS(EditorWindow, CWindow)
	public:

		EditorWindow();
		virtual ~EditorWindow();

	protected:

		void OnDrawGUI() override;

	};
    
} // namespace CE::Editor

#include "EditorWindow.rtti.h"
