#ifndef ASSETSVIEW_H
#define ASSETSVIEW_H

#include <QWidget>
#include <QItemSelection>
#include <QItemDelegate>

#include "Editor/EditorViewBase.h"

namespace Ui {
class AssetsView;
}

namespace CE::Editor
{
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

    signals:
        

    private slots:
        void OnFolderSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
        void OnAssetSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    private:
        AssetsViewFolderModel* folderModel = nullptr;
        AssetsViewContentModel* contentModel = nullptr;

        Vector<AssetViewItem*> assetItemWidgets{};

        Ui::AssetsView *ui;
    };

}

#include "AssetsView.rtti.h"


#endif // ASSETSVIEW_H
