#include "EditorSystem.h"

namespace CE::Editor
{
	EditorEngine::EditorEngine()
	{
		assetManager = CreateObject<EditorAssetManager>(this, "EditorAssetManager", OF_Transient);
	}

	EditorEngine::~EditorEngine()
	{
		
	}

} // namespace CE::Editor
