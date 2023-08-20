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

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void Construct() override;

	protected:

		void UpdateLayoutIfNeeded() override;

		void OnDrawGUI() override;

	};

} // namespace CE::Editor

#include "EditorPanel.rtti.h"
