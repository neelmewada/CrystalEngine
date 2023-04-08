#ifndef EXWIDGET_H
#define EXWIDGET_H

#include <QWidget>

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API ExWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit ExWidget(QWidget *parent = nullptr);
        virtual ~ExWidget();

    signals:

    protected:
        void focusInEvent(QFocusEvent* event) override;
        void focusOutEvent(QFocusEvent* event) override;

        void showEvent(QShowEvent* event) override;

    };

}

#endif // EXWIDGET_H
