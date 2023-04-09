#ifndef CONTEXTMENUREGULARITEM_H
#define CONTEXTMENUREGULARITEM_H

#include <QWidget>

namespace Ui {
class ContextMenuRegularItem;
}

namespace CE::Editor::Qt
{
    class ContextMenuWidget;

    class QTCOMPONENTS_API ContextMenuRegularItem : public QWidget
    {
        Q_OBJECT
    public:
        explicit ContextMenuRegularItem(ContextMenuWidget* parent = nullptr);
        ~ContextMenuRegularItem();

    protected:
        void mousePressEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;

        bool event(QEvent* event) override;

    public:
        void SetTitle(QString title);
        void SetIconResource(QString resource);

        void SetSubmenuEnabled(bool useSubmenu);

        ContextMenuWidget* GetSubmenu();

        void ShowSubmenu();
        void HideSubmenu();

        ContextMenuRegularItem* BindMouseClickSignal(QObject* slotObject, const char* slot);

        ContextMenuRegularItem* BindMouseClickSignal(Function<void(void)> func);

    signals:
        void OnMouseClicked();

    private:
        ContextMenuWidget* owner = nullptr;

        Ui::ContextMenuRegularItem *ui;
        ContextMenuWidget* submenu = nullptr;

        bool isHovered = false;
        bool mouseStartedInside = false;
    };

}

#endif // CONTEXTMENUREGULARITEM_H
