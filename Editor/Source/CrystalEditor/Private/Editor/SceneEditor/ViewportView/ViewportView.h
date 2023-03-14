#ifndef VIEWPORTVIEW_H
#define VIEWPORTVIEW_H

#include <QWindow>
#include <QWidget>

#include "Editor/EditorViewBase.h"

#include "System.h"

namespace Ui {
class ViewportView;
}

namespace CE::Editor
{
    class RenderViewport;

    class ViewportView : public EditorViewBase
    {
        Q_OBJECT
        CE_CLASS(ViewportView, EditorViewBase)
    public:
        explicit ViewportView(QWidget* parent = nullptr);
        ~ViewportView();

    protected:
        virtual void showEvent(QShowEvent* event) override;
        virtual void hideEvent(QHideEvent* event) override;

    private slots:
        void OnRenderLoop();

    private:
        Ui::ViewportView* ui;

        QTimer* renderLoopTimer = nullptr;
        bool stopRenderLoop = false;

        //RHIRenderTarget* renderTarget = nullptr;
        RHIViewport* viewportRHI = nullptr;
        RHIGraphicsCommandList* cmdList = nullptr;

        //QVulkanInstance* instance = nullptr;
        //QVulkanWindow* vulkanWindow = nullptr;
        RenderViewport* renderViewport = nullptr;
    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API,CE::Editor, ViewportView,
    CE_SUPER(CE::Editor::EditorViewBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)

#endif // VIEWPORTVIEW_H
