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
    class GameComponentDrawer;

    namespace Qt
    {
        class AddComponentWidget;
        class CardWidget;
    }

    class DetailsView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(DetailsView, EditorViewBase)
    public:
        explicit DetailsView(QWidget* parent = nullptr);
        ~DetailsView();

    private slots:
        void HandleCardContextMenu(u32 index, const QPoint& pos);

        void NameLabelApplyChanges();

        void on_addComponentButton_clicked();

        void OnAddComponentOfType(ClassType* componentType);

    public slots:
        void OnSceneSelectionChanged(Array<GameObject*> selected);

        void Refresh();

    signals:
        void GameObjectEntriesNeedRefresh(Array<GameObject*> gameObjects);

    protected:
        void focusInEvent(QFocusEvent* event) override;

    private:
        Ui::DetailsView* ui;
        Qt::AddComponentWidget* addComponentMenu = nullptr;
        Array<GameComponentDrawer*> drawers{};
        Array<Qt::CardWidget*> cards{};

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
