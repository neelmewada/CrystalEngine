
#include "Editor/EditorView.h"

#include <QStyleOption>
#include <QPainter>

namespace CE::Editor
{

    EditorView::EditorView(QWidget* parent) : QWidget(parent)
    {
        setObjectName("EditorView");
    }

    EditorView::~EditorView()
    = default;

    void EditorView::paintEvent(QPaintEvent* event)
    {
        QPainter painter(this);
        painter.setPen(::Qt::NoPen);
        painter.setBrush(QBrush(qApp->palette().color(QPalette::AlternateBase)));
        painter.drawRect(this->rect());

        QWidget::paintEvent(event);
    }

}


