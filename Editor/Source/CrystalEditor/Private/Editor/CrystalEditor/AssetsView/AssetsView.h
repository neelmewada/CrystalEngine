#ifndef ASSETSVIEW_H
#define ASSETSVIEW_H

#include <QWidget>

#include "Editor/EditorViewBase.h"

namespace Ui {
class AssetsView;
}

namespace CE::Editor
{

    CLASS()
    class AssetsView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(AssetsView, CE::Editor::EditorViewBase)
    public:
        explicit AssetsView(QWidget *parent = nullptr);
        ~AssetsView();

    private:
        Ui::AssetsView *ui;
    };

}

#include "AssetsView.rtti.h"


#endif // ASSETSVIEW_H
