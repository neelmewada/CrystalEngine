#include "TextureDetailsView.h"
#include "ui_TextureDetailsView.h"

namespace CE::Editor
{
    TextureDetailsView::TextureDetailsView(QWidget* parent)
        : EditorViewBase(parent)
        , ui(new Ui::TextureDetailsView)
    {
        ui->setupUi(this);

        setWindowTitle("Details");
    }

    TextureDetailsView::~TextureDetailsView()
    {
        delete ui;
    }
}