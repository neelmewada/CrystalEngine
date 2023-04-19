#pragma once

#include <QMainWindow>

#include "DockManager.h"
#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"
#include "DockAreaTabBar.h"

namespace CE
{
    class Scene;
}

namespace CE::Editor
{

    CLASS(Abstract)
    class CRYSTALEDITOR_API EditorWindow : public QMainWindow, public CE::Object
    {
        Q_OBJECT
        CE_CLASS(EditorWindow, CE::Object)
    public:
        explicit EditorWindow(QWidget* parent = nullptr);
        virtual ~EditorWindow();

        FUNCTION()
        virtual bool CanOpenAsset(AssetDatabaseEntry* assetEntry) = 0;

        FUNCTION()
        virtual bool OpenAsset(AssetDatabaseEntry* assetEntry) = 0;

        FUNCTION()
        virtual bool BrowseToAsset(AssetDatabaseEntry* assetEntry) = 0;
        
    protected:
        Scene* editorScene = nullptr;
    };
    
} // namespace CE

#include "EditorWindow.rtti.h"

