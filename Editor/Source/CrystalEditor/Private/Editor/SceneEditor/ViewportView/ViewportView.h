#ifndef VIEWPORTVIEW_H
#define VIEWPORTVIEW_H

#include <QWindow>
#include <QWidget>
#include <QVulkanInstance>

#include "Editor/EditorViewBase.h"

namespace Ui {
class ViewportView;
}

namespace CE::Editor
{

    class ViewportView : public EditorViewBase
    {
        Q_OBJECT

    public:
        explicit ViewportView(QWidget* parent = nullptr);
        ~ViewportView();

    private:
        Ui::ViewportView* ui;
        QVulkanInstance* instance = nullptr;
    };

}

#endif // VIEWPORTVIEW_H
