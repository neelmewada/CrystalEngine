#include "ExFrame.h"

namespace CE::Editor::Qt
{

    ExFrame::ExFrame(QWidget* parent)
        : QFrame(parent)
    {
        setFocusPolicy(::Qt::ClickFocus);
    }

    ExFrame::~ExFrame()
    {
    }

    void ExFrame::focusInEvent(QFocusEvent* event)
    {
        QFrame::focusInEvent(event);
    }

    void ExFrame::focusOutEvent(QFocusEvent* event)
    {
        QFrame::focusOutEvent(event);
    }

}
