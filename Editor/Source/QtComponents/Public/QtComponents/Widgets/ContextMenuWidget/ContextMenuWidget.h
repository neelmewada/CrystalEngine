#ifndef CONTEXTMENUWIDGET_H
#define CONTEXTMENUWIDGET_H

#include <QWidget>

namespace Ui {
class ContextMenuWidget;
}

#include "ContextMenuCategoryItem.h"
#include "ContextMenuRegularItem.h"

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API ContextMenuWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit ContextMenuWidget(QWidget *parent, ContextMenuWidget* owner = nullptr);
        ~ContextMenuWidget();

        void Clear();

        ContextMenuWidget* GetOwner() { return owner; }

        ContextMenuCategoryItem* AddCategoryLabel(QString title);

        ContextMenuRegularItem* AddRegularItem(QString title, QString icon = "");

    private:
        Ui::ContextMenuWidget *ui;

        ContextMenuWidget* owner = nullptr;
    };
}

#endif // CONTEXTMENUWIDGET_H
