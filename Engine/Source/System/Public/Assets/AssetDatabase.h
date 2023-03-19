#pragma once

#include "CoreMinimal.h"

namespace CE
{
    class Asset;

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

        Asset* asset = nullptr;

        CE::Array<AssetDatabaseEntry*> children{};
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

#if PAL_TRAIT_BUILD_EDITOR
        void LoadAssetDatabaseForEditor();
#else
        void LoadAssetDatabaseForRuntime();
#endif

        bool assetsLoaded = false;
        AssetDatabaseEntry* rootEntry = nullptr;
    };

} // namespace CE
