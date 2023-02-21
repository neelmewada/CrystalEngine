#include "DetailsView.h"
#include "ui_DetailsView.h"

#include "QtComponents/Widgets/CardWidget.h"

namespace CE::Editor
{
    DetailsView::DetailsView(QWidget* parent) :
        EditorViewBase(parent),
        ui(new Ui::DetailsView)
    {
        ui->setupUi(this);

        setWindowTitle("Details");

        auto card1 = new Qt::CardWidget(this);
        layout()->addWidget(card1);
        layout()->addWidget(new Qt::CardWidget(this));

        auto spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
        layout()->addItem(spacer);
    }

    DetailsView::~DetailsView()
    {
        delete ui;
    }
}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, DetailsView)
