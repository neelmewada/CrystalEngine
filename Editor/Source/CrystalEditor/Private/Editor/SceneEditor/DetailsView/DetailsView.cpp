#include "DetailsView.h"
#include "ui_DetailsView.h"

#include "QtComponents/Widgets/CardWidget.h"
#include "QtComponents/Widgets/AddComponentWidget.h"

#include "Drawers/GameComponentDrawer.h"

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
        for (auto drawer : drawers)
        {
            drawer->OnDisable();
            delete drawer;
        }
        drawers.Clear();

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
        this->selection = selected;
        
        Refresh();
    }

    void DetailsView::Refresh()
    {
        if (addComponentMenu != nullptr)
            addComponentMenu->hide();

        if (selection.GetSize() == 0)
        {
            ui->componentsContainer->setVisible(false);
            ui->headerFrame->setVisible(false);
            ui->addComponentButton->setVisible(false);
            return;
        }

        ui->componentsContainer->setVisible(true);
        ui->headerFrame->setVisible(true);
        ui->addComponentButton->setVisible(true);

        // Clear the component container
        auto layout = ui->componentsContainer->layout();
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != NULL)
        {
            delete item->widget();
            delete item;
        }


        if (selection.GetSize() > 1 || selection[0] == nullptr)
        {
            ui->nameInput->setText("-");
            ui->nameInput->setEnabled(true);
            ui->uuidValue->setText("-");
        }
        else
        {
            QString name = QString(selection[0]->GetName().GetCString());
            QString uuid = QStringLiteral("%1").arg((size_t)selection[0]->GetUuid());

            ui->nameInput->setText(name);
            ui->nameInput->setEnabled(true);
            ui->uuidValue->setText(uuid);
        }

        for (auto drawer : drawers)
        {
            drawer->OnDisable();
            delete drawer;
        }
        drawers.Clear();

        cards.Clear();
        
        if (selection.GetSize() != 1)
            return;

        for (int i = 0; i < selection[0]->GetComponentCount(); i++)
        {
            auto card = new Qt::CardWidget(this);
            ui->componentsContainer->layout()->addWidget(card);
            cards.Add(card);

            auto component = selection[0]->GetComponentAt(i);
            auto componentType = (ClassType*)component->GetType();

            auto componentName = componentType->GetLocalAttributeValue("Display");
            if (componentName.IsEmpty())
                componentName = componentType->GetName().GetLastComponent();
            card->SetCardTitle(componentName);
            
            auto drawerClass = GameComponentDrawer::GetGameComponentDrawerClassFor(componentType->GetTypeId());
            if (drawerClass == nullptr)
                continue;

            GameComponentDrawer* drawer = (GameComponentDrawer*)drawerClass->CreateDefaultInstance();
            if (drawer == nullptr)
                continue;

            drawer->SetTarget(componentType, component);

            drawer->OnEnable();

            drawer->CreateGUI(card->GetContentContainer()->layout());

            drawers.Add(drawer);
        }
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

        Refresh();
    }

    void DetailsView::focusInEvent(QFocusEvent* event)
    {
        EditorViewBase::focusInEvent(event);
    }

}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, DetailsView)
