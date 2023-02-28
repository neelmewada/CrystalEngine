#include "ExLineEdit.h"

#include "CoreMinimal.h"

#include <QFocusEvent>

namespace CE::Editor::Qt
{

    ExLineEdit::ExLineEdit(QWidget* parent)
        : QLineEdit(parent)
    {

    }

    ExLineEdit::~ExLineEdit()
    {

    }

    void ExLineEdit::focusInEvent(QFocusEvent* event)
    {
        QLineEdit::focusInEvent(event);
        emit OnFocusIn();
    }

    void ExLineEdit::focusOutEvent(QFocusEvent* event)
    {
        QLineEdit::focusOutEvent(event);
        emit OnFocusOut();
    }

}
