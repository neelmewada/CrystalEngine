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

        CE_SIGNAL(OnAssetDatabaseUpdated);

        CE_SIGNAL(OnAssetUpdated, IO::Path);
        
        CE_SIGNAL(OnNewSourceAssetAdded, IO::Path);

    private:

        // IFileWatchListener
        virtual void HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName) override;

        IO::FileWatcher* watcher = nullptr;
        IO::WatchID gameAssetsWatch{};
    };
    
} // namespace CE::Editor

#include "AssetManager.rtti.h"
