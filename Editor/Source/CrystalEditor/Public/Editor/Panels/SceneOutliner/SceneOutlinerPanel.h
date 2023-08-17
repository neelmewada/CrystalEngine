#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API SceneOutlinerPanel : public EditorPanel
	{
		CE_CLASS(SceneOutlinerPanel, EditorPanel)
	public:

		SceneOutlinerPanel();
		virtual ~SceneOutlinerPanel();

	protected:

		void Construct() override;

	};
    
} // namespace CE::Editor

#include "SceneOutlinerPanel.rtti.h"
