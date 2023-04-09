#ifndef CONTEXTMENUCATEGORYWIDGET_H
#define CONTEXTMENUCATEGORYWIDGET_H

#include <QWidget>

namespace Ui {
class ContextMenuCategoryItem;
}

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API ContextMenuCategoryItem : public QWidget
    {
        Q_OBJECT

    public:
        explicit ContextMenuCategoryItem(QWidget *parent = nullptr);
        ~ContextMenuCategoryItem();

        void SetTitle(QString text);

    private:
        Ui::ContextMenuCategoryItem *ui;
    };

}

#endif // CONTEXTMENUCATEGORYWIDGET_H
