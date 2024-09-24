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

	void EditorEngine::PostInitialize()
	{
		Super::PostInitialize();

	}

	void EditorEngine::PreShutdown()
	{
		Super::PreShutdown();

		auto app = FusionApplication::TryGet();

		if (app)
		{
			//app->Shutdown();
			//app->Destroy();
		}
	}

} // namespace CE::Editor
