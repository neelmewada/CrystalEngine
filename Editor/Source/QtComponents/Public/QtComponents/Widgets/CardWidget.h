#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include "CoreMinimal.h"

#include <QWidget>

namespace Ui {
class CardWidget;
}

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API CardWidget : public QWidget
    {
        Q_OBJECT

    public:
        explicit CardWidget(QWidget* parent = nullptr);
        ~CardWidget();

    public:

        CE_INLINE bool IsFoldToggled()
        {
            return foldToggle;
        }

        CE_INLINE void SetFoldToggle(bool toggle)
        {
            foldToggle = toggle;
            UpdateFoldout();
        }

    protected:
        void paintEvent(QPaintEvent* event) override;

        void UpdateFoldout();

    signals:
        void handleContextMenu(const QPoint& pos);

    private slots:
        void on_collapseButton_clicked();

        void on_headerLabel_clicked();

        void on_menuButton_clicked();

        void handleCustomContextMenu(const QPoint& pos);

    private:
        Ui::CardWidget* ui;
        bool foldToggle = true;

        bool mousePressedOnHeader = false;
    };

}

#endif // CARDWIDGET_H
