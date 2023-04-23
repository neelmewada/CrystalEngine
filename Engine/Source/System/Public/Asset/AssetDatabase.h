#pragma once

#include "CoreMinimal.h"
#include "Asset.h"

#include <mutex>

namespace CE
{

    class SYSTEM_API AssetDatabaseEntry
    {
    public:
        AssetDatabaseEntry() = default;
        ~AssetDatabaseEntry()
        {
            ClearChildren();
        }

        inline void ClearChildren()
        {
            for (int i = 0; i < children.GetSize(); i++)
            {
                delete children[i];
            }
            children.Clear();
        }

        enum class Type
        {
            Directory,
            Asset
        };

        enum class Category
        {
            EditorAssets,
            EditorShaders,
            EngineAssets,
            EngineShaders,
            GameAssets,
            GameShaders,
            PluginAssets,
            PluginShaders
        };

        bool IsRoot() const;

        IO::Path GetVirtualPath() const;

        bool IsEditorContentEntry() const;
        bool IsEngineContentEntry() const;
        bool IsGameContentEntry() const;
        bool IsPluginContentEntry() const;

        Name name{};
        String extension{};
        IO::Path virtualRelativePath{};
        Type entryType = Type::Directory;
        Category category = Category::EngineAssets;
        UUID uuid{};

        BuiltinAssetType builtinAssetType = BuiltinAssetType::None;
        TypeId assetClassId = 0;

        CE::Array<AssetDatabaseEntry*> children{};
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

        // - Internal API -

        CE_INLINE bool IsLoaded() const { return assetsLoaded; }

        CE_INLINE const AssetDatabaseEntry* GetRootEntry() const { return rootEntry; }

        void LoadDatabase();
        void ClearDatabase();
        void UnloadDatabase();

		ENGINE_CONST AssetDatabaseEntry* GetEntry(IO::Path virtualPath);

		ENGINE_CONST AssetDatabaseEntry* GetEntry(UUID assetUuid);

        // - Public API -

        Asset* LoadAssetAt(IO::Path virtualPath);

        UUID GetAssetUuid(IO::Path virtualPath);

    private:

        Asset* LoadRuntimeAssetAt(IO::Path virtualPath);

        UUID GetRuntimeAssetUuid(IO::Path virtualPath);

        AssetDatabaseEntry* SearchForEntry(AssetDatabaseEntry* searchRoot, IO::Path subVirtualPath);

        void LoadRuntimeAssetDatabase();

        std::mutex mut{};

        bool assetsLoaded = false;
        AssetDatabaseEntry* rootEntry = nullptr;

        HashMap<UUID, AssetDatabaseEntry*> uuidToAssetEntryMap{};

        friend class CE::Editor::AssetManager;
    };

} // namespace CE
