#pragma once

namespace CE::Editor
{

    CLASS()
	class CRYSTALEDITOR_API AssetBrowserPanel : public EditorPanel
	{
		CE_CLASS(AssetBrowserPanel, EditorPanel)
	public:

		AssetBrowserPanel();
		virtual ~AssetBrowserPanel();

		void Construct() override;

	protected:

		CContainerWidget* left = nullptr;
		CContainerWidget* right = nullptr;
	};

} // namespace CE::Editor

#include "AssetBrowserPanel.rtti.h"
