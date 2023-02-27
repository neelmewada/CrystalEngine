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

    void ExLineEdit::focusOutEvent(QFocusEvent* event)
    {
        emit OnFocusOut();
    }

}
