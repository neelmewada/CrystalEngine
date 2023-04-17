#include "TexturePreviewView.h"
#include "ui_TexturePreviewView.h"

#include "QtComponents/Widgets/ImageCanvas/ImageCanvas.h"

namespace CE::Editor
{

    TexturePreviewView::TexturePreviewView(QWidget* parent)
        : EditorViewBase(parent)
        , ui(new Ui::TexturePreviewView)
    {
        ui->setupUi(this);

        setWindowTitle("Preview");
    }

    TexturePreviewView::~TexturePreviewView()
    {
        delete ui;
    }

    void TexturePreviewView::SetImage(const CMImage& image)
    {
        ui->imageCanvas->SetImage(image);
    }

    void TexturePreviewView::Recenter()
    {
        ui->imageCanvas->Recenter();
    }

    void TexturePreviewView::RefreshPreview()
    {
        Recenter();
        //ui->imageCanvas->repaint();
    }

    void TexturePreviewView::resizeEvent(QResizeEvent *event)
    {
        EditorViewBase::resizeEvent(event);
        RefreshPreview();
    }

    void TexturePreviewView::SetImageCanvasChannel(Qt::ImageCanvasChannel channelFlags)
    {
        ui->imageCanvas->SetChannelFilter(channelFlags);
    }

    Qt::ImageCanvasChannel TexturePreviewView::GetImageCanvasChannelFlags()
    {
        return ui->imageCanvas->GetChannelFilter();
    }

}