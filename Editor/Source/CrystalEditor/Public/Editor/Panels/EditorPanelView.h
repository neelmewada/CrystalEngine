#pragma once

namespace CE::Editor
{

	CLASS()
	class CRYSTALEDITOR_API EditorPanelView : public CWidget
	{
		CE_CLASS(EditorPanelView, CWidget)
	public:

		EditorPanelView();
		virtual ~EditorPanelView();

		bool IsContainer() override { return true; }

	protected:

		void OnDrawGUI() override;

	};

    
} // namespace CE::Editor

#include "EditorPanelView.rtti.h"
