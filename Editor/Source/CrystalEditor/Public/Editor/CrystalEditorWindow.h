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

		void Construct() override;

	protected:

		AssetBrowserView* assetBrowserView = nullptr;
	};

} // namespace CE::Editor

#include "CrystalEditorWindow.rtti.h"
