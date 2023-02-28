#ifndef EXFRAME_H
#define EXFRAME_H

#include <QFrame>
#include <QObject>
#include <QWidget>

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API ExFrame : public QFrame
    {
        Q_OBJECT
    public:
        explicit ExFrame(QWidget* parent = nullptr);
        virtual ~ExFrame();

    protected:
        void focusInEvent(QFocusEvent* event) override;
        void focusOutEvent(QFocusEvent* event) override;
    };

}

#endif // EXFRAME_H
