#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>

namespace CE::Editor::Qt
{

    class ClickableLabel : public QLabel
    {
        Q_OBJECT
    public:
        explicit ClickableLabel(QWidget* parent = nullptr);

    signals:
        void clicked();

    protected:
        void mousePressEvent(QMouseEvent* event);

    };

}

#endif // CLICKABLELABEL_H
