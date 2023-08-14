#pragma once

namespace CE::Editor
{
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

		CPopup* demoPopup = nullptr;

		AssetBrowserPanel* assetBrowserPanel = nullptr;
	};

} // namespace CE::Editor

#include "CrystalEditorWindow.rtti.h"
