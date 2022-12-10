#ifndef TESTWINDOW_H
#define TESTWINDOW_H

#include <QMainWindow>

namespace Ui {
class TestWindow;
}

namespace CE::Editor
{

    class TestWindow : public QMainWindow
    {
        Q_OBJECT

    public:
        explicit TestWindow(QWidget* parent = nullptr);
        ~TestWindow();

    private:
        Ui::TestWindow* ui;
    };

}

#endif // TESTWINDOW_H
