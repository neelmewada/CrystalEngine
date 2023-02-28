#ifndef EXTREEVIEW_H
#define EXTREEVIEW_H

#include <QTreeView>
#include <QObject>

namespace CE::Editor::Qt
{

    class QTCOMPONENTS_API ExTreeView : public QTreeView
    {
        Q_OBJECT
    public:
        explicit ExTreeView(QWidget* parent = nullptr);
        virtual ~ExTreeView();

    protected:
        virtual void mouseDoubleClickEvent(QMouseEvent* event) override;
        virtual void mousePressEvent(QMouseEvent* event) override;
    };

}

#endif // EXTREEVIEW_H
