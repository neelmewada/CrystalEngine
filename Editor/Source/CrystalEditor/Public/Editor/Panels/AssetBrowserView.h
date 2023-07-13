#pragma once

namespace CE::Editor
{

    CLASS()
	class CRYSTALEDITOR_API AssetBrowserView : public EditorPanelView
	{
		CE_CLASS(AssetBrowserView, EditorPanelView)
	public:

		AssetBrowserView();
		virtual ~AssetBrowserView();

		void Construct() override;

	protected:


	};

} // namespace CE::Editor

#include "AssetBrowserView.rtti.h"
