#pragma once

namespace CE::Editor
{
	CLASS(AssetDef = "/Code/EditorSystem.CE::Editor::TextureAssetDefinition")
	class CRYSTALEDITOR_API TextureEditorWindow : public EditorWindow
	{
		CE_CLASS(TextureEditorWindow, EditorWindow)
	public:

		TextureEditorWindow();
		virtual ~TextureEditorWindow();

	};
    
} // namespace CE::Editor

#include "TextureEditorWindow.rtti.h"
