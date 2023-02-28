#include "DetailsView.h"
#include "ui_DetailsView.h"

#include "QtComponents/Widgets/CardWidget.h"
#include "QtComponents/Widgets/AddComponentWidget.h"

#include <QMenu>

namespace CE::Editor
{
    DetailsView::DetailsView(QWidget* parent) :
        EditorViewBase(parent),
        ui(new Ui::DetailsView)
    {
        ui->setupUi(this);

        setWindowTitle("Details");

        auto spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
        layout()->addItem(spacer);

        addComponentMenu = new Qt::AddComponentWidget(this);

        ui->componentsContainer->setVisible(false);
        ui->headerFrame->setVisible(false);
        ui->addComponentButton->setVisible(false);

        // Name Input
        connect(ui->nameInput, &QLineEdit::returnPressed, this, &DetailsView::NameLabelApplyChanges);
        connect(ui->nameInput, &Qt::ExLineEdit::OnFocusOut, this, &DetailsView::NameLabelApplyChanges);

        // Add Component Menu
        connect(addComponentMenu, &Qt::AddComponentWidget::AddComponentOfType, this, &DetailsView::OnAddComponentOfType);
    }

    DetailsView::~DetailsView()
    {
        delete ui;
    }

    void DetailsView::NameLabelApplyChanges()
    {
        auto newName = ui->nameInput->text().toStdString();
        auto newNameCStr = newName.c_str();

        for (int i = 0; i < selection.GetSize(); i++)
        {
            if (selection[i] == nullptr)
                continue;

            selection[i]->SetName(newNameCStr);
        }

        emit GameObjectEntriesNeedRefresh(selection);
    }

    void DetailsView::OnSceneSelectionChanged(Array<GameObject*> selected)
    {
        if (addComponentMenu != nullptr)
            addComponentMenu->hide();

        if (selected.GetSize() == 0)
        {
            ui->componentsContainer->setVisible(false);
            ui->headerFrame->setVisible(false);
            ui->addComponentButton->setVisible(false);
            return;
        }

        ui->componentsContainer->setVisible(true);
        ui->headerFrame->setVisible(true);
        ui->addComponentButton->setVisible(true);

        // Clear the component cards
        auto layout = ui->componentsContainer->layout();
        for (int i = layout->count() - 1; i >= 0; i--)
        {
            layout->removeItem(layout->itemAt(i));
        }

        if (selected.GetSize() > 1 || selected[0] == nullptr)
        {
            ui->nameInput->setText("-");
            ui->nameInput->setEnabled(true);
            ui->uuidValue->setText("-");
        }
        else
        {
            QString name = QString(selected[0]->GetName().GetCString());
            QString uuid = QStringLiteral("%1").arg((size_t)selected[0]->GetUuid());
            
            ui->nameInput->setText(name);
            ui->nameInput->setEnabled(true);
            ui->uuidValue->setText(uuid);
        }

        auto card1 = new Qt::CardWidget(this);
        QObject::connect(card1, &Qt::CardWidget::handleContextMenu, this, &DetailsView::HandleCardContextMenu);
        ui->componentsContainer->layout()->addWidget(card1);

        this->selection = selected;
    }

    void DetailsView::HandleCardContextMenu(const QPoint& pos)
    {
        QMenu contextMenu(tr("Context menu"), this);

        auto font = contextMenu.font();
        font.setPointSize(12);
        contextMenu.setFont(font);

        QAction action1("Delete Component", this);
        contextMenu.addAction(&action1);

        contextMenu.exec(pos);
    }


    void DetailsView::on_addComponentButton_clicked()
    {
        addComponentMenu->move(mapToGlobal(ui->addComponentButton->pos()) + QPoint(0, 25));
        addComponentMenu->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        addComponentMenu->setFixedSize(ui->addComponentButton->size().width(), 300);
        addComponentMenu->show();
    }

    void DetailsView::OnAddComponentOfType(ClassType* componentType)
    {
        if (selection.GetSize() == 0)
            return;

        for (auto go : selection)
        {
            go->AddComponent(componentType);
        }
    }

    void DetailsView::focusInEvent(QFocusEvent* event)
    {
        EditorViewBase::focusInEvent(event);
    }

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, DetailsView)
