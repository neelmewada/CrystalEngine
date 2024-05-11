#pragma once

#include "SceneDataModel.h"

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API SceneHierarchyWindow : public MinorEditorWindow
	{
		CE_CLASS(SceneHierarchyWindow, MinorEditorWindow)
	public:

		SceneHierarchyWindow();
		virtual ~SceneHierarchyWindow();

		void SetScene(CE::Scene* scene);

	private:

		void Construct() override;

		FIELD(ReadOnly)
		CTreeView* hierarchyTreeView = nullptr;

		FIELD()
		CE::Scene* openScene = nullptr;

		FIELD()
		SceneDataModel* dataModel = nullptr;

	};

} // namespace CE::Editor

#include "SceneHierarchyWindow.rtti.h"