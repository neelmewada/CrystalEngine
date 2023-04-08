#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QWidget>

namespace Ui {
class GridWidget;
}

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API GridWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit GridWidget(QWidget* parent = nullptr);
        ~GridWidget();

        void AddWidget(QWidget* widget);
        void RemoveWidget(QWidget* widget);

        void ClearWidgets();

        void SetMinimumItemWidth(int width);
        
        void Update();

    private:
        QWidgetList widgetList{};

        int minWidth = 100;

        Ui::GridWidget* ui;
    };

}

#endif // GRIDWIDGET_H
