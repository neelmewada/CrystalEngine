#ifndef DETAILSVIEW_H
#define DETAILSVIEW_H

#include <QWidget>

#include "CoreMinimal.h"
#include "GameFramework/GameFramework.h"

namespace Ui {
class DetailsView;
}

namespace CE::Editor
{
    class DetailsView 
        : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(DetailsView, EditorViewBase)
    public:
        explicit DetailsView(QWidget* parent = nullptr);
        ~DetailsView();

    private slots:
        void HandleCardContextMenu(const QPoint& pos);

        void NameLabelApplyChanges();

    public slots:
        void OnSceneSelectionChanged(Array<GameObject*> selected);

    signals:
        void GameObjectEntriesNeedRefresh(Array<GameObject*> gameObjects);

    private:
        Ui::DetailsView* ui;

        CE::Array<CE::GameObject*> selection{};
    };
}

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, DetailsView,
    CE_SUPER(CE::Editor::EditorViewBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // DETAILSVIEW_H
