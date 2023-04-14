#include "TextureEditorWindow.h"
#include "ui_TextureEditorWindow.h"

namespace CE::Editor
{

    TextureEditorWindow::TextureEditorWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::TextureEditorWindow)
    {
        ui->setupUi(this);

        setWindowTitle("Texture Editor");
    }

    TextureEditorWindow::~TextureEditorWindow()
    {
        delete ui;
    }

}
