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
            ClearChildren();
        }

        inline void ClearChildren()
        {
            children.Clear();
        }

        enum class Type
        {
            Directory,
            Asset
        };

        enum class Category
        {
            EngineAssets,
            EngineShaders,
            GameAssets,
            GameShaders,
            PluginAssets,
            PluginShaders
        };

        bool IsRoot() { return virtualPath.IsEmpty() && children.GetSize() == 0 && parent == nullptr; }

        Name name{};
        String extension{};
        IO::Path virtualRelativePath{};
        IO::Path virtualPath{};
        Type entryType = Type::Directory;
        Category category = Category::EngineAssets;

        CE::Array<AssetDatabaseEntry> children{};
        AssetDatabaseEntry* parent = nullptr;

        // In run-time build, assets are packed into several archives.
        u32 assetPackIndex = 0;
    };

    namespace Editor
    {
        class AssetManager;
    }

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

        CE_INLINE const AssetDatabaseEntry* GetRootEntry() const { return rootEntry; }

        void LoadDatabase();
        void ClearDatabase();
        void UnloadDatabase();

        const AssetDatabaseEntry* GetEntry(IO::Path virtualPath);

    private:

        const AssetDatabaseEntry* SearchForEntry(AssetDatabaseEntry* searchRoot, IO::Path subVirtualPath);

        void LoadRuntimeAssetDatabase();

        std::mutex mut{};

        bool assetsLoaded = false;
        AssetDatabaseEntry* rootEntry = nullptr;

        friend class CE::Editor::AssetManager;
    };

} // namespace CE
