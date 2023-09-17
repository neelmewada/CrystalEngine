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

		FIELD()
		Scene* currentScene = nullptr;

		AssetBrowserPanel* assetBrowserPanel = nullptr;
		RenderViewportPanel* viewportPanel = nullptr;
		DetailsPanel* detailsPanel = nullptr;
		SceneOutlinerPanel* sceneOutlinerPanel = nullptr;
	};

} // namespace CE::Editor

#include "SceneEditorWindow.rtti.h"
