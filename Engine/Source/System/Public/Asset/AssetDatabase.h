#pragma once

#include "CoreMinimal.h"
#include "Asset.h"

#include <mutex>

namespace CE
{

    class AssetDatabaseEntry
    {
    public:
        ~AssetDatabaseEntry()
        {
            for (auto child : children)
            {
                delete child;
            }
            children.Clear();
        }

        enum class Type
        {
            Directory,
            Asset
        };

        Name name{};
        IO::Path virtualPath{};
        Type entryType = Type::Directory;
        CE::Array<AssetDatabaseEntry*> children{};

        // In run-time build, assets are packed into several archives.
        u32 assetPackIndex = 0;
    };

    class SYSTEM_API AssetDatabase : IO::IFileWatchListener
    {
    private:
        AssetDatabase() = default;
        ~AssetDatabase() = default;
    public:

        static AssetDatabase& Get()
        {
            static AssetDatabase instance{};
            return instance;
        }

        void Initialize();
        void Shutdown();

        CE_INLINE bool IsLoaded() const { return assetsLoaded; }

        void LoadDatabase();
        void UnloadDatabase();

    private:

        void LoadAssetDatabaseForEditor();
        void LoadAssetDatabaseForRuntime();

        IO::FileWatcher* watcher = nullptr;
        IO::WatchID gameAssetsWatch{};

        std::mutex mut{};

        bool assetsLoaded = false;
        AssetDatabaseEntry* rootEntry = nullptr;

        // IFileWatchListener
        virtual void HandleFileAction(IO::WatchID watchId, IO::Path directory, String fileName, IO::FileAction fileAction, String oldFileName) override;

    };

} // namespace CE
