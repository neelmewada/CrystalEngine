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
    class AssetsView;

    class CRYSTALEDITOR_API AssetsViewItemDelegate : public QItemDelegate
    {
        Q_OBJECT
    public:
        explicit AssetsViewItemDelegate(AssetsView* parent = nullptr);
        ~AssetsViewItemDelegate();

        QWidget* createEditor(QWidget* parent,
            const QStyleOptionViewItem& option,
            const QModelIndex& index) const override final;

        void setEditorData(QWidget* editor, const QModelIndex& index) const override final;

        void SetRenameItemIndex(const QModelIndex& index)
        {
            renameIndex = index;
        }

    private slots:
        void OnNameInputEdited(QString newString);

    private:
        QModelIndex renameIndex{};
        AssetsView* assetsView = nullptr;
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
        void OnContextMenuRenamePressed();
        void OnContextMenuDuplicatePressed();
        void OnContextMenuMoveToPressed();
        void OnContextMenuDeletePressed();

        // Misc

        void ReimportAssets(Array<AssetDatabaseEntry*> assetsToReimport);

    private:
        AssetDatabaseEntry* currentDirectory = nullptr;
        AssetsViewFolderModel* folderModel = nullptr;
        AssetsViewContentModel* contentModel = nullptr;
        Qt::ContextMenuWidget* contextMenu = nullptr;
        AssetsViewItemDelegate* itemDelegate = nullptr;

        Vector<AssetViewItem*> assetItemWidgets{};

        Ui::AssetsView *ui;

        friend class AssetsViewItemDelegate;
    };

}

#include "AssetsView.rtti.h"


#endif // ASSETSVIEW_H
