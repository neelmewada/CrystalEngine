
#include "Editor/EditorViewBase.h"

#include <QStyleOption>
#include <QPainter>

namespace CE::Editor
{

    EditorViewBase::EditorViewBase(QWidget* parent) : QWidget(parent)
    {
        setObjectName("EditorViewBase");
    }

    EditorViewBase::~EditorViewBase()
    {

    }

    void EditorViewBase::paintEvent(QPaintEvent* event)
    {
        QPainter p(this);
        p.setPen(::Qt::NoPen);
        p.setBrush(QBrush(qApp->palette().color(QPalette::AlternateBase)));
        p.drawRect(this->rect());

        QWidget::paintEvent(event);
    }

}


