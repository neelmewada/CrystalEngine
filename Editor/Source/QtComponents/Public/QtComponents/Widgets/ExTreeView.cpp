#include "ExTreeView.h"

#include "CoreMinimal.h"

#include <QMouseEvent>

namespace CE::Editor::Qt
{

    ExTreeView::ExTreeView(QWidget* parent)
        : QTreeView(parent)
    {

    }

    ExTreeView::~ExTreeView()
    {
    }

    void ExTreeView::mouseDoubleClickEvent(QMouseEvent* event)
    {
        QTreeView::mouseDoubleClickEvent(event);
    }

    void ExTreeView::mousePressEvent(QMouseEvent* event)
    {
        QTreeView::mousePressEvent(event);
    }

}
