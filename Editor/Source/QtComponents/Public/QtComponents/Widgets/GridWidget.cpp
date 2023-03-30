#include "GridWidget.h"
#include "ui_GridWidget.h"

#include "CoreMinimal.h"

namespace CE::Editor::Qt
{

    GridWidget::GridWidget(QWidget* parent) :
        QWidget(parent),
        ui(new Ui::GridWidget)
    {
        ui->setupUi(this);

        
    }

    GridWidget::~GridWidget()
    {
        delete ui;
    }

    void GridWidget::AddWidget(QWidget* widget)
    {
        widgetList.append(widget);
    }

    void GridWidget::RemoveWidget(QWidget* widget)
    {
        widgetList.removeOne(widget);
    }

    void GridWidget::ClearWidgets()
    {
        widgetList.clear();
    }

    void GridWidget::SetMinimumItemWidth(int width)
    {
        this->minWidth = width;
    }

    void GridWidget::Update()
    {
        QLayoutItem* item;
        while ((item = ui->gridLayout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }

        int numItems = widgetList.count();

        if (numItems == 0)
            return;

        int widthPerItem = Math::Min(minWidth, widgetList.at(0)->width());

        int maxNumItemsInRow = Math::Max(1, width() / widthPerItem);
        
        int numCols = Math::Min(numItems / maxNumItemsInRow, maxNumItemsInRow);
        int numRows = numItems / numCols + 1;
        
        for (int r = 0; r < numRows; r++)
        {
            for (int c = 0; c < numCols; c++)
            {
                int index = r * numCols + c;
                if (index >= widgetList.count())
                    continue;

                ui->gridLayout->addWidget(widgetList.at(index), r, c);
            }
        }
    }
}
