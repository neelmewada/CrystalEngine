#include "AddComponentWidget.h"
#include "ui_AddComponentWidget.h"

#include "AddComponentWidgetModel.h"

namespace CE::Editor::Qt
{


    AddComponentWidget::AddComponentWidget(QWidget* parent) :
        ExWidget(parent),
        ui(new Ui::AddComponentWidget)
    {
        ui->setupUi(this);
        setWindowFlags(::Qt::Popup);

        model = new AddComponentWidgetModel(this);
        ui->treeView->setModel(model);

        connect(ui->searchInput, &Qt::ExLineEdit::textChanged, this, &AddComponentWidget::OnSearchTextChanged);
        connect(ui->treeView, &QTreeView::doubleClicked, this, &AddComponentWidget::OnItemDoubleClicked);
    }

    AddComponentWidget::~AddComponentWidget()
    {
        delete ui;
    }

    void AddComponentWidget::OnSearchTextChanged(const QString& newText)
    {
        model->SetSearchText(ui->searchInput->text());
    }

    void AddComponentWidget::UpdateComponentRegistry()
    {
        model->UpdateComponentRegistry();
    }

    void AddComponentWidget::OnItemDoubleClicked(const QModelIndex& index)
    {
        CE::Name name{};
        auto componentType = model->GetComponentTypeFromIndex(index, name);

        if (componentType == nullptr)
        {
            CE_LOG(Info, All, "Failed to find component of name: {}", name);
            return;
        }

        emit AddComponentOfType(componentType);
        hide();
    }

    void AddComponentWidget::showEvent(QShowEvent* event)
    {
        ui->searchInput->setFocus();
    }
}
