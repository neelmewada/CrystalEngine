#include "TextureEditorWindow.h"
#include "ui_TextureEditorWindow.h"

#include "TexturePreviewView/TexturePreviewView.h"
#include "TextureDetailsView/TextureDetailsView.h"

#include "CoreMedia.h"

namespace CE::Editor
{

    TextureEditorWindow::TextureEditorWindow(QWidget *parent)
        : EditorWindowBase(parent)
        , ui(new Ui::TextureEditorWindow)
    {
        ui->setupUi(this);

        setWindowTitle("Texture Editor");

        using namespace ads;

        CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);

        dockManager = new CDockManager(this);

        // **********************************
        // Texture Preview View
        previewView = new TexturePreviewView(this);
        previewViewDockWidget = new CDockWidget(previewView->windowTitle());
        previewViewDockWidget->setWidget(previewView);

        // **********************************
        // Texture Details View
        detailsView = new TextureDetailsView(this);
        detailsViewDockWidget = new CDockWidget(detailsView->windowTitle());
        detailsViewDockWidget->setWidget(detailsView);
        detailsView->resize(200, detailsView->height());

        // **********************************
        // Add the dock widget to the dock manager
        dockManager->addDockWidget(ads::CenterDockWidgetArea, previewViewDockWidget);
        dockManager->addDockWidget(ads::RightDockWidgetArea, detailsViewDockWidget);

        // ***********************************
        // ToolBar
        ui->toolActionSave->setIcon(QIcon(":/Editor/Icons/save-colored"));

    }

    TextureEditorWindow::~TextureEditorWindow()
    {
        delete ui;
    }

    bool TextureEditorWindow::CanOpenAsset(AssetDatabaseEntry* assetEntry)
    {
        if (assetEntry == nullptr)
            return false;
        auto assetClass = Asset::GetAssetClassFor(assetEntry->extension);
        if (assetEntry->category == AssetDatabaseEntry::Category::GameAssets && assetClass != nullptr && assetClass->GetTypeId() == TYPEID(TextureAsset))
        {
            return true;
        }
        return false;
    }

    bool TextureEditorWindow::OpenAsset(AssetDatabaseEntry* assetEntry)
    {
        if (!CanOpenAsset(assetEntry))
            return false;
        SetTextureAsset(assetEntry);
        return true;
    }

    void TextureEditorWindow::SetTextureAsset(IO::Path assetPath)
    {
        IO::Path basePath = ProjectSettings::Get().GetEditorProjectDirectory();
        auto relativePath = IO::Path::GetRelative(assetPath, basePath);

        auto entry = (AssetDatabaseEntry*)AssetDatabase::Get().GetEntry(relativePath);
        if (entry == nullptr)
            return;

        SetTextureAsset(assetEntry);
    }

    void TextureEditorWindow::SetTextureAsset(AssetDatabaseEntry* assetEntry)
    {
        IO::Path basePath = ProjectSettings::Get().GetEditorProjectDirectory();

        this->assetEntry = assetEntry;

        if (assetEntry != nullptr)
        {
            auto texturePath = basePath / assetEntry->GetVirtualPath().ReplaceExtension(assetEntry->extension);
            if (texturePath.Exists())
            {
                CMImage image = CMImage::LoadFromFile(texturePath);
                previewView->SetImage(image);
                previewView->Recenter();
            }
        }
    }

    void TextureEditorWindow::resizeEvent(QResizeEvent *event)
    {
        EditorWindowBase::resizeEvent(event);
        previewView->RefreshPreview();
    }

}
