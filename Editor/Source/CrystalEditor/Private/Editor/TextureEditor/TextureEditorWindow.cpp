#include "TextureEditorWindow.h"
#include "ui_TextureEditorWindow.h"

#include "TexturePreviewView/TexturePreviewView.h"
#include "TextureDetailsView/TextureDetailsView.h"

#include <QToolButton>
#include <QMenu>
#include <QMetaObject>

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

        dockManager->setStyleSheet(dockManager->styleSheet() + "\n" + qApp->styleSheet());

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
        ui->toolActionBrowse->setIcon(QIcon(":/Editor/Icons/search-folder"));

        // Horizontal Spacer
        auto spacer = new QWidget();
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->toolBar->addWidget(spacer);

        ui->toolBar->addSeparator();

        // View Settings
        auto viewSettingsAction = ui->toolBar->addAction(QIcon(":/Editor/Icons/settings"), "View Settings");
        viewSettingsBtn = qobject_cast<QToolButton*>(ui->toolBar->widgetForAction(viewSettingsAction));
        if (viewSettingsBtn != nullptr)
        {
            viewSettingsBtn->setFont(ui->toolActionSave->font());
            viewSettingsBtn->setPopupMode(QToolButton::InstantPopup);
            viewSettingsBtn->removeAction(viewSettingsAction);
            showPreviewView = viewSettingsBtn->addAction("Show Preview", [this]
            {
                previewViewDockWidget->toggleView(true);
            });
            showDetailsView = viewSettingsBtn->addAction("Show Details", [this]
            {
                detailsViewDockWidget->toggleView(true);
            });
        }
    }

    TextureEditorWindow::~TextureEditorWindow()
    {
        targetImage.Free();

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

    bool TextureEditorWindow::BrowseToAsset(CE::AssetDatabaseEntry* assetEntry)
    {
        return false;
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
                targetImage.Free();
                targetImage = CMImage::LoadFromFile(texturePath);
                previewView->SetImage(targetImage);
                previewView->Recenter();
            }
        }
    }

    void TextureEditorWindow::resizeEvent(QResizeEvent *event)
    {
        EditorWindowBase::resizeEvent(event);
        previewView->RefreshPreview();
    }

    void TextureEditorWindow::on_toolActionBrowse_triggered()
    {
        if (assetEntry == nullptr)
            return;

        CE_PUBLISH(CrystalEditorBus, BrowseToAsset, assetEntry);
    }

    void TextureEditorWindow::on_toolActionRedChannel_triggered()
    {
        auto flags = previewView->GetImageCanvasChannelFlags();
        if (ui->toolActionRedChannel->isChecked())
            previewView->SetImageCanvasChannel(flags | Qt::ImageCanvasChannel::Red);
        else
            previewView->SetImageCanvasChannel(flags & ~Qt::ImageCanvasChannel::Red);
    }

    void TextureEditorWindow::on_toolActionGreenChannel_triggered()
    {
        auto flags = previewView->GetImageCanvasChannelFlags();
        if (ui->toolActionGreenChannel->isChecked())
            previewView->SetImageCanvasChannel(flags | Qt::ImageCanvasChannel::Green);
        else
            previewView->SetImageCanvasChannel(flags & ~Qt::ImageCanvasChannel::Green);
    }

    void TextureEditorWindow::on_toolActionBlueChannel_triggered()
    {
        auto flags = previewView->GetImageCanvasChannelFlags();
        if (ui->toolActionBlueChannel->isChecked())
            previewView->SetImageCanvasChannel(flags | Qt::ImageCanvasChannel::Blue);
        else
            previewView->SetImageCanvasChannel(flags & ~Qt::ImageCanvasChannel::Blue);
    }

    void TextureEditorWindow::on_toolActionAlphaChannel_triggered()
    {
        auto flags = previewView->GetImageCanvasChannelFlags();
        if (ui->toolActionAlphaChannel->isChecked())
            previewView->SetImageCanvasChannel(flags | Qt::ImageCanvasChannel::Alpha);
        else
            previewView->SetImageCanvasChannel(flags & ~Qt::ImageCanvasChannel::Alpha);
    }

}
