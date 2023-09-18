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

		int DrawNode(Node* node);

		FIELD()
		Scene* scene = nullptr;

		friend class SceneOutlinerPanel;
	};

	CLASS()
	class CRYSTALEDITOR_API SceneOutlinerPanel : public EditorPanel
	{
		CE_CLASS(SceneOutlinerPanel, EditorPanel)
	public:

		SceneOutlinerPanel();
		virtual ~SceneOutlinerPanel();

		void SetScene(Scene* scene);

	protected:

		void Construct() override;

		void OnDrawGUI() override;

		FIELD()
		Scene* scene = nullptr;

		FIELD()
		SceneOutlinerTree* tree = nullptr;
	};
    
} // namespace CE::Editor

#include "SceneOutlinerPanel.rtti.h"
