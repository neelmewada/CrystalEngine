
#include "System.h"

namespace CE
{

	void AssetDatabase::Initialize()
	{
		rootEntry = new AssetDatabaseEntry;
		rootEntry->name = "Root";
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

#if PAL_TRAIT_BUILD_EDITOR
	void AssetDatabase::LoadAssetDatabaseForEditor()
	{
		const auto& projectSettings = ProjectSettings::Get();
		auto engineAssetsDir = PlatformDirectories::GetEngineDir() / "Assets";

		auto projectDir = projectSettings.editorProjectDirectory;

		if (projectDir.IsEmpty() || !projectDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load asset database! Invalid project path: {}", projectDir);
			return;
		}

		if (!engineAssetsDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load engine asset database! Invalid engine assets path: {}", engineAssetsDir);
			return;
		}

		// Load Project Assets

		assetsLoaded = true;
	}
#else
	void AssetDatabase::LoadAssetDatabaseForRuntime()
	{
		// TODO: Runtime asset packaging system

		assetsLoaded = true;
	}
#endif

} // namespace CE
