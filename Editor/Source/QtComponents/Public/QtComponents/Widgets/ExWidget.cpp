#include "ExWidget.h"

#include "CoreMinimal.h"

namespace CE::Editor::Qt
{

    ExWidget::ExWidget(QWidget *parent)
        : QWidget(parent)
    {
        setFocusPolicy(::Qt::ClickFocus);
    }

    ExWidget::~ExWidget()
    {
    }

    void ExWidget::focusInEvent(QFocusEvent* event)
    {
        CE_LOG(Info, All, "Focus In");
        QWidget::focusInEvent(event);
    }

    void ExWidget::focusOutEvent(QFocusEvent* event)
    {
        CE_LOG(Info, All, "Focus Out");
        QWidget::focusOutEvent(event);
    }

    void ExWidget::showEvent(QShowEvent* event)
    {
        QWidget::showEvent(event);
    }

}
