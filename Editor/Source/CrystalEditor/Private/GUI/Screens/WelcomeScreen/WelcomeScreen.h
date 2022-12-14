#ifndef WELCOMESCREEN_H
#define WELCOMESCREEN_H

#include <QDialog>
#include <QTimer>

namespace Ui {
class WelcomeScreen;
}

namespace CE::Editor
{
    class WelcomeScreen : public QDialog
    {
        Q_OBJECT

    public:
        explicit WelcomeScreen(QWidget* parent = nullptr);
        ~WelcomeScreen();

    private Q_SLOTS:
        void OnTimer();

    private:
        Ui::WelcomeScreen* ui;
        
        QTimer* timer = nullptr;
    };

}

#endif // WELCOMESCREEN_H
