#include "EditorSystem.h"

namespace CE::Editor
{
	EditorAssetManager::EditorAssetManager()
	{
		
	}

	EditorAssetManager::~EditorAssetManager()
	{
		
	}

	EditorAssetManager* EditorAssetManager::Get()
	{
		return (EditorAssetManager*)AssetManager::Get();
	}

	void EditorAssetManager::Initialize()
	{
		Super::Initialize();

		assetRegistry->listener = this;
	}

	void EditorAssetManager::Shutdown()
	{
		assetRegistry->listener = nullptr;

		Super::Shutdown();
	}

    void EditorAssetManager::Tick(f32 deltaTime)
    {
		Super::Tick(deltaTime);

		if (mainWindow == nullptr)
			mainWindow = PlatformApplication::Get()->GetMainWindow();

		
    }

} // namespace CE::Editor
