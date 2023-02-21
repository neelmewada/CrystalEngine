#include "ClickableLabel.h"

namespace CE::Editor::Qt
{

    

    ClickableLabel::ClickableLabel(QWidget* parent)
        : QLabel(parent)
    {

    }

    void ClickableLabel::mousePressEvent(QMouseEvent* event)
    {
        emit clicked();
    }

}
