#pragma once

namespace CE::Editor
{
    CLASS()
	class CRYSTALEDITOR_API SceneEditorWindow : public EditorWindow
	{
		CE_CLASS(SceneEditorWindow, EditorWindow)
	public:

		SceneEditorWindow();
		virtual ~SceneEditorWindow();

		virtual bool CanBeClosed() override { return false; }

		void OpenScene(Scene* scene);
		void CloseScene();

	protected:

		void Construct() override;

		FUNCTION()
		void OnNewNodeMenuItemClicked(CMenuItem* menuItem);

		FIELD()
		Scene* currentScene = nullptr;

		FIELD()
		Node* selectedNode = nullptr;

		AssetBrowserPanel* assetBrowserPanel = nullptr;
		RenderViewportPanel* viewportPanel = nullptr;
		DetailsPanel* detailsPanel = nullptr;
		SceneOutlinerPanel* sceneOutlinerPanel = nullptr;

		CMenu* newNodeMenu = nullptr;
	};

} // namespace CE::Editor

#include "SceneEditorWindow.rtti.h"
