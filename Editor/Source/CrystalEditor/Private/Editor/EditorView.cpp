
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
    {

    }

    void EditorView::paintEvent(QPaintEvent* event)
    {
        QPainter p(this);
        p.setPen(::Qt::NoPen);
        p.setBrush(QBrush(qApp->palette().color(QPalette::AlternateBase)));
        p.drawRect(this->rect());

        QWidget::paintEvent(event);
    }

}


