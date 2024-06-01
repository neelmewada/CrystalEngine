#pragma once

#include "SceneDataModel.h"

namespace CE::Editor
{
	DECLARE_SCRIPT_EVENT(ItemSelectionEvent, CItemSelection* selection);
	DECLARE_SCRIPT_EVENT(ActorSelectionEvent, Actor* actor);

	CLASS()
	class CRYSTALEDITOR_API SceneHierarchyWindow : public MinorEditorWindow
	{
		CE_CLASS(SceneHierarchyWindow, MinorEditorWindow)
	public:

		SceneHierarchyWindow();
		virtual ~SceneHierarchyWindow();

		void SetScene(CE::Scene* scene);

		CTreeView* GetHierarchyTreeView() const { return hierarchyTreeView; }

		// - Events -

		FIELD()
		ActorSelectionEvent onActorSelected;

		FIELD()
		ItemSelectionEvent onSceneSelectionChanged;


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