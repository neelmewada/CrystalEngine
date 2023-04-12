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
    class CRYSTALEDITOR_API EditorWindowBase : public QMainWindow, public CE::Object
    {
        Q_OBJECT
        CE_CLASS(EditorWindowBase, CE::Object)
    public:
        explicit EditorWindowBase(QWidget* parent = nullptr);
        virtual ~EditorWindowBase();

        FUNCTION()
        virtual bool CanOpenAsset(AssetDatabaseEntry* assetEntry) = 0;

        FUNCTION()
        virtual bool OpenAsset(AssetDatabaseEntry* assetEntry) = 0;
        
    protected:
        Scene* editorScene = nullptr;
    };
    
} // namespace CE

#include "EditorWindowBase.rtti.h"

