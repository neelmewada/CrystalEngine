#ifndef VIEWPORTVIEW_H
#define VIEWPORTVIEW_H

#include <QWindow>
#include <QWidget>

#include "Editor/EditorView.h"
#include "GUI/ViewportWindow.h"

#include "System.h"

namespace Ui {
class ViewportView;
}

namespace CE::Editor
{

    class ViewportView : public EditorView
    {
        Q_OBJECT
        CE_CLASS(ViewportView, EditorView)
    public:
        explicit ViewportView(QWidget* parent = nullptr);
        ~ViewportView();

    protected:
        virtual void showEvent(QShowEvent* event) override;
        virtual void hideEvent(QHideEvent* event) override;
        virtual void resizeEvent(QResizeEvent* event) override;

        void CreateViewport();
        void DestroyViewport();

    private slots:
        void OnRenderLoop();

    private:
        Ui::ViewportView* ui;

        QTimer* renderLoopTimer = nullptr;
        bool stopRenderLoop = false;

        RHIViewport* viewportRHI = nullptr;
        RHIGraphicsCommandList* cmdList = nullptr;

        ViewportWindow* renderViewport = nullptr;
        QWidget* renderViewportWidget = nullptr;
    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API,CE::Editor, ViewportView,
    CE_SUPER(CE::Editor::EditorView),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // VIEWPORTVIEW_H
