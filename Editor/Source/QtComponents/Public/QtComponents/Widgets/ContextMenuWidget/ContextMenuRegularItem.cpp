#include "ContextMenuRegularItem.h"
#include "ui_ContextMenuRegularItem.h"

#include "QtComponents.h"

#include <QMouseEvent>

namespace CE::Editor::Qt
{

    ContextMenuRegularItem::ContextMenuRegularItem(ContextMenuWidget* parent) :
        QWidget(parent), owner(parent),
        ui(new Ui::ContextMenuRegularItem)
    {
        ui->setupUi(this);
        ui->arrowIcon->setVisible(false);

        this->setAttribute(::Qt::WA_Hover);
    }

    ContextMenuRegularItem::~ContextMenuRegularItem()
    {
        delete ui;
    }

    void ContextMenuRegularItem::mousePressEvent(QMouseEvent* event)
    {
        QWidget::mousePressEvent(event);

        auto mousePos = event->globalPos();
        auto thisPos = parentWidget() != nullptr ? parentWidget()->mapToGlobal(pos()) : mapToGlobal(pos());
        auto size = this->size();

        QRect boundRect{ thisPos, size };

        if (boundRect.contains(mousePos))
        {
            mouseStartedInside = true;
        }
        else
        {
            mouseStartedInside = false;
        }
    }

    void ContextMenuRegularItem::mouseReleaseEvent(QMouseEvent* event)
    {
        QWidget::mouseReleaseEvent(event);

        if (!mouseStartedInside)
            return;

        mouseStartedInside = false;

        auto mousePos = event->globalPos();
        auto thisPos = parentWidget() != nullptr ? parentWidget()->mapToGlobal(pos()) : mapToGlobal(pos());
        auto size = this->size();

        QRect boundRect{ thisPos, size };

        if (boundRect.contains(mousePos))
        {
            // On Click inside
            if (submenu != nullptr)
            {
                ShowSubmenu();
            }
            else
            {
                auto owner = this->owner;
                while (owner != nullptr)
                {
                    owner->hide();
                    owner = owner->GetOwner();
                }
                emit OnMouseClicked();
            }
        }
    }

    bool ContextMenuRegularItem::event(QEvent* event)
    {
        switch (event->type())
        {
        case QEvent::HoverEnter:
            isHovered = true;
            break;
        case QEvent::HoverLeave:
            isHovered = false;
            break;
        }

        return QWidget::event(event);
    }

    void ContextMenuRegularItem::SetTitle(QString title)
    {
        ui->label->setText(title);
    }

    void ContextMenuRegularItem::SetIconResource(QString resource)
    {
        ui->icon->setStyleSheet("border-image: url(" + resource + ");");
    }

    void ContextMenuRegularItem::SetSubmenuEnabled(bool useSubmenu)
    {
        if (!useSubmenu)
        {
            delete submenu;
            submenu = nullptr;
            ui->arrowIcon->setVisible(false);
            return;
        }

        if (submenu == nullptr)
        {
            submenu = new ContextMenuWidget(owner, owner);
            ui->arrowIcon->setVisible(true);
        }
    }

    ContextMenuWidget* ContextMenuRegularItem::GetSubmenu()
    {
        return submenu;
    }

    void ContextMenuRegularItem::ShowSubmenu()
    {
        if (submenu == nullptr)
            return;

        auto thisPos = parentWidget() != nullptr ? parentWidget()->mapToGlobal(pos()) : mapToGlobal(pos());
        auto thisSize = this->size();
        auto screenSize = qApp->primaryScreen()->size();

        auto submenuSize = submenu->size();
        auto submenuRightPos = QPoint(thisPos.x() + thisSize.width(), thisPos.y());
        auto submenuLeftPos = QPoint(thisPos.x() - submenuSize.width(), thisPos.y());

        auto submenuTargetPos = submenuRightPos;

        if (submenuRightPos.x() + submenuSize.width() > screenSize.width())
        {
            submenuTargetPos = submenuLeftPos;
        }

        if (submenuTargetPos.y() + submenuSize.height() > screenSize.height())
        {
            submenuTargetPos.setY(screenSize.height() - submenuSize.height() - 10);
        }

        submenu->move(submenuTargetPos);
        submenu->show();
    }

    void ContextMenuRegularItem::HideSubmenu()
    {
        if (submenu == nullptr)
            return;

        submenu->hide();
    }

    ContextMenuRegularItem* ContextMenuRegularItem::BindMouseClickSignal(QObject* slotObject, const char* slot)
    {
        QObject::connect(this, SIGNAL(OnMouseClicked()), slotObject, slot);
        return this;
    }

    ContextMenuRegularItem* ContextMenuRegularItem::BindMouseClickSignal(Function<void(void)> func)
    {
        QObject::connect(this, &ContextMenuRegularItem::OnMouseClicked, func);
        return this;
    }

}
