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

		bool IsLayoutCalculationRoot() override final { return true; }

		virtual bool ShouldCreateToolBar() { return true; }
		virtual bool ShouldCreateMenuBar() { return true; }

	protected:

		void Construct() override;

		void OnDrawGUI() override;

	};
    
} // namespace CE::Editor

#include "EditorWindow.rtti.h"
