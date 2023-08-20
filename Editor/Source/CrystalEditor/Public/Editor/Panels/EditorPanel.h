#pragma once

namespace CE::Editor
{
	
    CLASS()
	class CRYSTALEDITOR_API EditorPanel : public CWindow
	{
		CE_CLASS(EditorPanel, CWindow)
	public:

		EditorPanel();
		virtual ~EditorPanel();

		bool IsLayoutCalculationRoot() override final { return true; }

		void Construct() override;

	protected:

		void OnDrawGUI() override;

	};

} // namespace CE::Editor

#include "EditorPanel.rtti.h"
