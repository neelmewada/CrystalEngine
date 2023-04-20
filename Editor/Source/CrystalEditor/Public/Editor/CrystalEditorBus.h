#pragma once

#include "Messaging/MessageBus.h"

namespace CE
{
    class AssetDatabaseEntry;
}

namespace CE::Editor
{

    class CRYSTALEDITOR_API CrystalEditorRequests
    {
    public:
        CrystalEditorRequests() = default;
        virtual ~CrystalEditorRequests() = default;

        virtual void OpenAsset(AssetDatabaseEntry* assetEntry) = 0;

        virtual void BrowseToAsset(AssetDatabaseEntry* assetEntry) = 0;

        virtual EditorWindow* GetEditorWindow(ClassType* editorWindowType) = 0;

        virtual ads::CDockWidget* GetEditorWindowDockWidget(ClassType* editorWindowType) = 0;

        static void RegisterEditorWindowClassForBuiltinAsset(ClassType* editorWindowClass, BuiltinAssetType builtinAssetType);
        static void RegisterEditorWindowClassForAsset(ClassType* editorWindowClass, ClassType* assetClass);

        static ClassType* GetBuiltinAssetEditorWindow(BuiltinAssetType builtinAssetType);
        static ClassType* GetAssetEditorWindow(TypeId assetClassType);
        static ClassType* GetAssetEditorWindow(ClassType* assetClass);

        static CrystalEditorRequests* Get();

    protected:
        static HashMap<BuiltinAssetType, ClassType*> builtinAssetEditors;
        static HashMap<TypeId, ClassType*> assetEditors;
    };

    CE_EVENT_BUS(CRYSTALEDITOR_API, CrystalEditorBus, CrystalEditorRequests);

} // namespace CE::Editor

#define CE_REGISTER_BUILTIN_ASSET_EDITOR(builtinAssetType, EditorWindowClass) \
CE::Editor::CrystalEditorRequests::RegisterEditorWindowClassForBuiltinAsset(EditorWindowClass::Type(), CE::BuiltinAssetType::builtinAssetType);

#define CE_REGISTER_ASSET_EDITOR(AssetClass, EditorWindowClass) \
CE::Editor::CrystalEditorRequests::RegisterEditorWindowClassForAsset(EditorWindowClass::Type(), AssetClass::Type())
