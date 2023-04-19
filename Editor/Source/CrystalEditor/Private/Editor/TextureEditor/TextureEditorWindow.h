#ifndef TEXTUREEDITORWINDOW_H
#define TEXTUREEDITORWINDOW_H

#include <QMainWindow>

#include "CoreMedia.h"

#include "Editor/EditorWindow.h"

namespace Ui {
class TextureEditorWindow;
}

namespace CE::Editor
{
    class TexturePreviewView;
    class TextureDetailsView;

    CLASS()
    class TextureEditorWindow
            : public EditorWindow
    {
        Q_OBJECT
        CE_CLASS(TextureEditorWindow, CE::Editor::EditorWindow)
    public:
        explicit TextureEditorWindow(QWidget *parent = nullptr);
        ~TextureEditorWindow();

        // EditorWindowBase overrides

        bool CanOpenAsset(AssetDatabaseEntry* assetEntry) override;

        bool OpenAsset(AssetDatabaseEntry* assetEntry) override;

        bool BrowseToAsset(CE::AssetDatabaseEntry* assetEntry) override;

        // Public API

        void SetTextureAsset(IO::Path assetPath);
        void SetTextureAsset(AssetDatabaseEntry* assetEntry);

    private slots:
        void on_toolActionBrowse_triggered();

        void on_toolActionRedChannel_triggered();

        void on_toolActionGreenChannel_triggered();

        void on_toolActionBlueChannel_triggered();

        void on_toolActionAlphaChannel_triggered();

    private:

        void resizeEvent(QResizeEvent *event) override;

    private:
        Ui::TextureEditorWindow *ui;

        ads::CDockManager* dockManager = nullptr;

        AssetDatabaseEntry* assetEntry = nullptr;

        CMImage targetImage{};

        // View Settings Popup
        QToolButton* viewSettingsBtn = nullptr;
        QAction* showPreviewView = nullptr;
        QAction* showDetailsView = nullptr;

        // Sub Views
        TexturePreviewView* previewView = nullptr;
        ads::CDockWidget* previewViewDockWidget = nullptr;

        TextureDetailsView* detailsView = nullptr;
        ads::CDockWidget* detailsViewDockWidget = nullptr;
    };

}

#include "TextureEditorWindow.rtti.h"

#endif // TEXTUREEDITORWINDOW_H
