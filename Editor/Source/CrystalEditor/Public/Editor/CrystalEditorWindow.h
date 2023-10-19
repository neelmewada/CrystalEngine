#pragma once

namespace CE::Editor
{
	class EditorWindow;
	class SceneEditorWindow;
	class ProjectSettingsWindow;

    CLASS()
	class CRYSTALEDITOR_API CrystalEditorWindow : public CWindow
	{
		CE_CLASS(CrystalEditorWindow, CWindow)
	public:

		CrystalEditorWindow();
		virtual ~CrystalEditorWindow();

		static CrystalEditorWindow* Get();

		void Construct() override;

		// - Public API -

		void ShowProjectSettingsWindow();

	protected:

		void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

		CPopup* assetImportPopup = nullptr;

		SceneEditorWindow* sceneEditorWindow = nullptr;
		ProjectSettingsWindow* settingsWindow = nullptr;
	};

} // namespace CE::Editor

#include "CrystalEditorWindow.rtti.h"
