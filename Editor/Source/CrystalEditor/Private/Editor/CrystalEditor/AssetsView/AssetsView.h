#ifndef ASSETSVIEW_H
#define ASSETSVIEW_H

#include <QWidget>
#include <QItemSelection>
#include <QItemDelegate>

#include "Editor/EditorViewBase.h"

namespace Ui {
class AssetsView;
}

namespace CE
{
    class AssetDatabaseEntry;
}

namespace CE::Editor
{
    namespace Qt
    {
        class ContextMenuWidget;
    }

    class AssetsViewFolderModel;
    class AssetsViewContentModel;
    class AssetViewItem;

    class CRYSTALEDITOR_API AssetsViewItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit AssetsViewItemDelegate(QObject* parent = nullptr);
        ~AssetsViewItemDelegate();

        QWidget* createEditor(QWidget* parent,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override final;
    };

    CLASS()
    class CRYSTALEDITOR_API AssetsView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(AssetsView, CE::Editor::EditorViewBase)
    public:
        explicit AssetsView(QWidget *parent = nullptr);
        ~AssetsView();

        virtual void resizeEvent(QResizeEvent* event) override;

    private:
        void UpdateContentView();

        void PopulateCreateContextEntries(Qt::ContextMenuWidget* contextMenu);

    private slots:

        FUNCTION(Event)
        void OnAssetDatabaseUpdated();

        void OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
        void OnAssetSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

        void OnAssetViewItemDoubleClicked(const QModelIndex& index);
        void OnAssetViewItemContextMenuRequested(const QPoint& pos);

        // Context Menu Actions
        
        void OnContextMenuNewFolderPressed();
        void OnContextMenuDeletePressed();

    private:
        AssetDatabaseEntry* currentDirectory = nullptr;
        AssetsViewFolderModel* folderModel = nullptr;
        AssetsViewContentModel* contentModel = nullptr;
        Qt::ContextMenuWidget* contextMenu = nullptr;

        Vector<AssetViewItem*> assetItemWidgets{};

        Ui::AssetsView *ui;
    };

}

#include "AssetsView.rtti.h"


#endif // ASSETSVIEW_H
