#pragma once

#include "CoreMinimal.h"
#include "Asset.h"

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
        Type entryType = Type::Directory;
        CE::Array<AssetDatabaseEntry*> children{};

        // In run-time build, assets are packed into several different archives.
        u32 assetPackIndex = 0;
    };

    class SYSTEM_API AssetDatabase
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

        bool assetsLoaded = false;
        AssetDatabaseEntry* rootEntry = nullptr;
    };

} // namespace CE
