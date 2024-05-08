#pragma once

namespace CE::Editor
{
	CLASS()
	class CRYSTALEDITOR_API SceneEditorWindow final : public EditorWindow
	{
		CE_CLASS(SceneEditorWindow, EditorWindow)
	public:

		SceneEditorWindow();

		virtual ~SceneEditorWindow();

	private:

		void Construct() override;

	};

} // namespace CE::Editor

#include "SceneEditorWindow.rtti.h"