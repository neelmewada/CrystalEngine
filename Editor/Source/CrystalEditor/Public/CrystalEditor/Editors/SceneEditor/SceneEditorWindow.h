#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API SceneEditorWindow final : public EditorWindow
	{
		CE_CLASS(SceneEditorWindow, EditorWindow)
	public:

		SceneEditorWindow();

		virtual ~SceneEditorWindow();

		ViewportWindow* GetViewportWindow() const { return viewportWindow; }

	private:

		void Construct() override;

		FIELD()
		ViewportWindow* viewportWindow = nullptr;

		FIELD()
		AssetBrowserWindow* assetBrowserWindow = nullptr;

		FIELD()
		DetailsWindow* detailsWindow = nullptr;

		FIELD()
		RendererSubsystem* rendererSubsystem = nullptr;

	};

} // namespace CE::Editor

#include "SceneEditorWindow.rtti.h"