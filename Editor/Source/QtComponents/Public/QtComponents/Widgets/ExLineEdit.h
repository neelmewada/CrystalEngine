#ifndef EXLINEEDIT_H
#define EXLINEEDIT_H

#include <QLineEdit>
#include <QObject>
#include <QWidget>

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API ExLineEdit : public QLineEdit
    {
        Q_OBJECT
    public:
        explicit ExLineEdit(QWidget* parent = nullptr);
        ~ExLineEdit();

    protected:
        void focusInEvent(QFocusEvent* event) override;
        void focusOutEvent(QFocusEvent* event) override;

    signals:
        void OnFocusIn();
        void OnFocusOut();
    };

}

#endif // EXLINEEDIT_H
