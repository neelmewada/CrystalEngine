#include "EditorSystem.h"

namespace CE::Editor
{
	EditorEngine::EditorEngine()
	{
		assetManager = CreateDefaultSubobject<EditorAssetManager>("EditorAssetManager", OF_Transient);
	}

	EditorEngine::~EditorEngine()
	{
		
	}

} // namespace CE::Editor
