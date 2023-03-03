#ifndef VIEWPORTVIEW_H
#define VIEWPORTVIEW_H

#include <QWindow>
#include <QWidget>

#include "Editor/EditorViewBase.h"

namespace Ui {
class ViewportView;
}

namespace CE::Editor
{

    class ViewportView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(ViewportView, EditorViewBase)
    public:
        explicit ViewportView(QWidget* parent = nullptr);
        ~ViewportView();

    private:
        Ui::ViewportView* ui;
    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API,CE::Editor, ViewportView,
    CE_SUPER(CE::Editor::EditorViewBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // VIEWPORTVIEW_H
