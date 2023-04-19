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
    class SceneEditorWindow;

    namespace Qt
    {
        class AddComponentWidget;
        class CardWidget;
    }

    CLASS()
    class DetailsView : public EditorView
    {
        Q_OBJECT
        CE_CLASS(DetailsView, CE::Editor::EditorView)
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

        friend class SceneEditorWindow;
    };
}

#include "DetailsView.rtti.h"


#endif // DETAILSVIEW_H
