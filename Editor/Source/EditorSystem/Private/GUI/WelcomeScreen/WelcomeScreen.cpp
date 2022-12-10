
#include "GUI/WelcomeScreen/WelcomeScreen.h"
#include "ui_WelcomeScreen.h"

#include <QStyleFactory>

namespace CE::Editor
{

    WelcomeScreen::WelcomeScreen(QWidget* parent) :
        QDialog(parent),
        ui(new Ui::WelcomeScreen)
    {
        ui->setupUi(this);

        setWindowFlags(::Qt::Window | ::Qt::FramelessWindowHint);
    }

    WelcomeScreen::~WelcomeScreen()
    {
        delete ui;
    }

}
