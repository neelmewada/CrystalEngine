#pragma once

#include "System.h"

#include <QWindow>

namespace CE::Editor
{
    
    class EDITORCORE_API ViewportWindow : public QWindow
    {
    public:
        ViewportWindow(QWindow* parent = nullptr);
        virtual ~ViewportWindow();

        void* GetWindowHandle()
        {
            return (void*)winId();
        }

    protected:
        virtual void showEvent(QShowEvent* event) override;
        virtual void hideEvent(QHideEvent* event) override;

    private:
    };

} // namespace CE::Editor
