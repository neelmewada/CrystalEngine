
#include "System.h"

namespace CE
{

	void AssetDatabase::Initialize()
	{
		
	}

	void AssetDatabase::Shutdown()
	{
		delete watcher;
		watcher = nullptr;

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
		auto editorDir = PlatformDirectories::GetEditorDir();

#if PAL_TRAIT_BUILD_EDITOR
		auto gameDir = projectSettings.editorProjectDirectory / "Game";
#else
		auto gameDir = PlatformDirectories::GetGameDir();
#endif

		if (gameDir.IsEmpty() || !gameDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load asset database! Invalid project path: {}", gameDir);
			return;
		}

		if (!engineDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load engine asset database! Invalid engine assets path: {}", engineDir);
		}

		if (!editorDir.Exists())
		{
			CE_LOG(Error, All, "Failed to load editor asset database! Invalid editor assets path: {}", editorDir);
		}

		// Load Project Assets

		assetsLoaded = true;

		if (watcher == nullptr)
		{
			watcher = new IO::FileWatcher();
			gameAssetsWatch = watcher->AddWatcher(gameDir, this, true);

			watcher->Watch();
		}
	}

	void AssetDatabase::LoadAssetDatabaseForRuntime()
	{
		// TODO: Runtime asset packaging system

		assetsLoaded = true;
	}

	void AssetDatabase::HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName)
	{
		std::lock_guard<std::mutex> guard(mut);
		CE_LOG(Info, All, "Watcher Action: {}/{}  || action: {}", directory, fileName, fileAction);
	}

} // namespace CE
