#ifndef ASSETSVIEW_H
#define ASSETSVIEW_H

#include <QWidget>
#include <QItemSelection>

#include "Editor/EditorViewBase.h"

namespace Ui {
class AssetsView;
}

namespace CE::Editor
{
    class AssetsViewFolderModel;

    CLASS()
    class CRYSTALEDITOR_API AssetsView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(AssetsView, CE::Editor::EditorViewBase)
    public:
        explicit AssetsView(QWidget *parent = nullptr);
        ~AssetsView();

        virtual void resizeEvent(QResizeEvent* event) override;

    private slots:
        void OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    private:
        AssetsViewFolderModel* folderModel = nullptr;

        Ui::AssetsView *ui;
    };

}

#include "AssetsView.rtti.h"


#endif // ASSETSVIEW_H
