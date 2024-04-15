#pragma once

namespace CE::Editor
{
	CLASS(Abstract)
	class CRYSTALEDITOR_API EditorWindow : public CDockWindow
	{
		CE_CLASS(EditorWindow, CDockWindow)
	public:

		EditorWindow();
		virtual ~EditorWindow();

	protected:

		void Construct() override;

		FIELD()
		EditorToolBar* toolBar = nullptr;

	};

} // namespace CE::Editor

#include "EditorWindow.rtti.h"