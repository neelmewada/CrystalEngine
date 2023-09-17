#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API SceneOutlinerTree : public CWidget
	{
		CE_CLASS(SceneOutlinerTree, CWidget)
	public:

		SceneOutlinerTree();
		virtual ~SceneOutlinerTree();

	protected:

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void OnDrawGUI() override;

	};

	CLASS()
	class CRYSTALEDITOR_API SceneOutlinerPanel : public EditorPanel
	{
		CE_CLASS(SceneOutlinerPanel, EditorPanel)
	public:

		SceneOutlinerPanel();
		virtual ~SceneOutlinerPanel();

	protected:

		void Construct() override;

		void OnDrawGUI() override;
	};
    
} // namespace CE::Editor

#include "SceneOutlinerPanel.rtti.h"
