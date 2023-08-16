#pragma once

namespace CE::Editor
{
	class EditorWindow;
	class SceneEditorWindow;

    CLASS()
	class CRYSTALEDITOR_API CrystalEditorWindow : public CWindow
	{
		CE_CLASS(CrystalEditorWindow, CWindow)
	public:

		CrystalEditorWindow();
		virtual ~CrystalEditorWindow();

		static CrystalEditorWindow* Get();

		void Construct() override;

		void ShowDemoPopup();

	protected:

		void OnDrawGUI() override;

		CPopup* assetImportPopup = nullptr;

		SceneEditorWindow* sceneEditorWindow = nullptr;
	};

} // namespace CE::Editor

#include "CrystalEditorWindow.rtti.h"
