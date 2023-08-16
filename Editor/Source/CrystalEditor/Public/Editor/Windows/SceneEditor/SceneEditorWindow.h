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

	protected:

		void Construct() override;

		AssetBrowserPanel* assetBrowserPanel = nullptr;
		RenderViewportPanel* renderViewport = nullptr;
	};

} // namespace CE::Editor

#include "SceneEditorWindow.rtti.h"
