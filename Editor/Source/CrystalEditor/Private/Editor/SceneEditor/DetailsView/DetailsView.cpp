#include "DetailsView.h"
#include "ui_DetailsView.h"

#include "QtComponents/Widgets/CardWidget.h"

#include <QMenu>

namespace CE::Editor
{
    DetailsView::DetailsView(QWidget* parent) :
        EditorViewBase(parent),
        ui(new Ui::DetailsView)
    {
        ui->setupUi(this);

        setWindowTitle("Details");

        auto card1 = new Qt::CardWidget(this);
        QObject::connect(card1, &Qt::CardWidget::handleContextMenu, this, &DetailsView::HandleCardContextMenu);
        layout()->addWidget(card1);

        auto card2 = new Qt::CardWidget(this);
        QObject::connect(card2, &Qt::CardWidget::handleContextMenu, this, &DetailsView::HandleCardContextMenu);
        layout()->addWidget(card2);

        auto spacer = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
        layout()->addItem(spacer);
    }

    DetailsView::~DetailsView()
    {
        delete ui;
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
}

CE_RTTI_CLASS_IMPL(CRYSTALEDITOR_API, CE::Editor, DetailsView)
