#pragma once

#include "CoreMinimal.h"

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API AssetManager : public CE::Object, public IO::IFileWatchListener
    {
        CE_CLASS(AssetManager, CE::Object)
    private:
        AssetManager();
    public:
        ~AssetManager();

        static AssetManager& Get()
        {
            static AssetManager instance{};
            return instance;
        }

        void LoadEditorAssetDatabase();

        CE_SIGNAL(OnRequestDeselectAssets);

        CE_SIGNAL(OnAssetDatabaseUpdated);

        CE_SIGNAL(OnAssetUpdated, IO::Path);
        
        CE_SIGNAL(OnNewSourceAssetAdded, IO::Path);

        // - Public API -

        void MoveAssetEntryToDirectory(AssetDatabaseEntry* entry, const IO::Path& directoryRelativePath);

        void DeleteAssetDatabaseEntry(AssetDatabaseEntry* entry);

        bool ImportExternalAssets(Array<IO::Path> externalPaths, AssetDatabaseEntry* directoryEntry);

    private:

        // IFileWatchListener
        void HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName) override;

        IO::FileWatcher* watcher = nullptr;
        IO::WatchID gameAssetsWatch{};
    };
    
} // namespace CE::Editor

#include "AssetManager.rtti.h"
