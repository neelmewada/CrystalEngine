#pragma once

#include "CoreMinimal.h"

namespace CE::Editor
{

    class EDITORCORE_API AssetManager : IO::IFileWatchListener
    {
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

    private:

        // IFileWatchListener
        virtual void HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName) override;

        IO::FileWatcher* watcher = nullptr;
        IO::WatchID gameAssetsWatch{};
    };
    
} // namespace CE::Editor
