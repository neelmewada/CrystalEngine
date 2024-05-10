#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API SceneHierarchyWindow : public MinorEditorWindow
	{
		CE_CLASS(SceneHierarchyWindow, MinorEditorWindow)
	public:

		SceneHierarchyWindow();
		virtual ~SceneHierarchyWindow();

	private:

		void Construct() override;

	};

} // namespace CE::Editor

#include "SceneHierarchyWindow.rtti.h"