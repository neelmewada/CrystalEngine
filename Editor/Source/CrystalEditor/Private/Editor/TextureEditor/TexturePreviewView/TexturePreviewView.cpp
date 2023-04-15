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

}