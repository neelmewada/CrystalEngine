#ifndef ASSETSVIEW_H
#define ASSETSVIEW_H

#include <QWidget>

#include "Editor/EditorViewBase.h"

namespace Ui {
class AssetsView;
}

namespace CE::Editor
{

    class AssetsView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(AssetsView, EditorViewBase)
    public:
        explicit AssetsView(QWidget *parent = nullptr);
        ~AssetsView();

    private:
        Ui::AssetsView *ui;
    };

}


CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, AssetsView,
    CE_SUPER(CE::Editor::EditorViewBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)


#endif // ASSETSVIEW_H
