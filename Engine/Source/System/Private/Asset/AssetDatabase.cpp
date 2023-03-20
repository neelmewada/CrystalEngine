
#include "System.h"

namespace CE
{

	void AssetDatabase::Initialize()
	{
		
	}

	void AssetDatabase::Shutdown()
	{
		UnloadDatabase();
	}

	void AssetDatabase::LoadDatabase()
	{
		if (assetsLoaded)
			return;

#if PAL_TRAIT_BUILD_EDITOR
		LoadAssetDatabaseForEditor();
#else
		LoadAssetDatabaseForRuntime();
#endif
	}

	void AssetDatabase::UnloadDatabase()
	{
		assetsLoaded = false;

		delete rootEntry;
		rootEntry = nullptr;
	}

	void AssetDatabase::LoadAssetDatabaseForEditor()
	{
		UnloadDatabase();

		rootEntry = new AssetDatabaseEntry;
		rootEntry->name = "Root";

		const auto& projectSettings = ProjectSettings::Get();
		auto engineDir = PlatformDirectories::GetEngineDir();

#if PAL_TRAIT_BUILD_EDITOR
		auto gameDir = projectSettings.editorProjectDirectory / "Game/Assets";
#else
		auto gameDir = PlatformDirectories::GetGameDir() / "Assets";
#endif

		if (gameDir.IsEmpty() || !gameDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load asset database! Invalid project path: {}", gameDir);
			return;
		}

		if (!engineDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load engine asset database! Invalid engine assets path: {}", engineDir);
			return;
		}

		// Load Project Assets

		assetsLoaded = true;
	}

	void AssetDatabase::LoadAssetDatabaseForRuntime()
	{
		// TODO: Runtime asset packaging system

		assetsLoaded = true;
	}

} // namespace CE
