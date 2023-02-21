#ifndef DETAILSVIEW_H
#define DETAILSVIEW_H

#include <QWidget>

namespace Ui {
class DetailsView;
}

namespace CE::Editor
{
    class DetailsView 
        : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(DetailsView, EditorViewBase)
    public:
        explicit DetailsView(QWidget* parent = nullptr);
        ~DetailsView();

    private:
        Ui::DetailsView* ui;
    };
}

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, DetailsView,
    CE_SUPER(CE::Editor::EditorViewBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // DETAILSVIEW_H
