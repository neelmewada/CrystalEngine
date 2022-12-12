
#include "Events/EditorSystemEventBus.h"

#include "GUI/Screens/WelcomeScreen/WelcomeScreen.h"
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

        timer = new QTimer(this);

        QTimer::singleShot(2000, this, &WelcomeScreen::OnTimer);
    }

    WelcomeScreen::~WelcomeScreen()
    {
        delete ui;
    }

    void WelcomeScreen::OnTimer()
    {
        hide();

        MBUS_EVENT(EditorSystemEventBus, OnWelcomeScreenTimeout);
    }
}
