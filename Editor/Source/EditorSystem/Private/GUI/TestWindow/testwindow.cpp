#include "testwindow.h"
#include "ui_testwindow.h"

namespace CE::Editor
{
    TestWindow::TestWindow(QWidget* parent) :
        QMainWindow(parent),
        ui(new Ui::TestWindow)
    {
        ui->setupUi(this);
    }

    TestWindow::~TestWindow()
    {
        delete ui;
    }
}
