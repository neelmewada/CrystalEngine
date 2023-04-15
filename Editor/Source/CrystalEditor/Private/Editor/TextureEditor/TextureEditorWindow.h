#ifndef TEXTUREEDITORWINDOW_H
#define TEXTUREEDITORWINDOW_H

#include <QMainWindow>

#include "Editor/EditorWindowBase.h"

namespace Ui {
class TextureEditorWindow;
}

namespace CE::Editor
{
    class TexturePreviewView;

    CLASS()
    class TextureEditorWindow
            : public EditorWindowBase
    {
        Q_OBJECT
        CE_CLASS(TextureEditorWindow, CE::Editor::EditorWindowBase)
    public:
        explicit TextureEditorWindow(QWidget *parent = nullptr);
        ~TextureEditorWindow();

        // EditorWindowBase overrides

        bool CanOpenAsset(AssetDatabaseEntry* assetEntry) override;

        bool OpenAsset(AssetDatabaseEntry* assetEntry) override;

        // Public API

        void SetTextureAsset(IO::Path assetPath);
        void SetTextureAsset(AssetDatabaseEntry* assetEntry);

    private:
        Ui::TextureEditorWindow *ui;

        ads::CDockManager* dockManager = nullptr;

        AssetDatabaseEntry* assetEntry = nullptr;

        // Sub Views
        TexturePreviewView* previewView = nullptr;
        ads::CDockWidget* previewViewDockWidget = nullptr;
    };

}

#include "TextureEditorWindow.rtti.h"

#endif // TEXTUREEDITORWINDOW_H
