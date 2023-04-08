#ifndef ADDCOMPONENTWIDGET_H
#define ADDCOMPONENTWIDGET_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "QtComponents/Widgets/ExWidget.h"

namespace Ui {
class AddComponentWidget;
}

namespace CE::Editor::Qt
{
    class AddComponentWidgetModel;

    class QTCOMPONENTS_API AddComponentWidget : public ExWidget
    {
        Q_OBJECT

    public:
        explicit AddComponentWidget(QWidget* parent = nullptr);
        ~AddComponentWidget();

    signals:
        void AddComponentOfType(ClassType* componentType);

    private slots:
        void OnSearchTextChanged(const QString& newText);

        void OnItemDoubleClicked(const QModelIndex& index);

    protected:
        void showEvent(QShowEvent* event) override;

    public slots:
        void UpdateComponentRegistry();

    private:

        Ui::AddComponentWidget* ui;
        AddComponentWidgetModel* model = nullptr;
    };

}

#endif // ADDCOMPONENTWIDGET_H
