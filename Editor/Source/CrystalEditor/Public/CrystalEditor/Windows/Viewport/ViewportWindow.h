#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API ViewportWindow : public MinorEditorWindow
    {
        CE_CLASS(ViewportWindow, MinorEditorWindow)
    public:

        ViewportWindow();
        virtual ~ViewportWindow();

    private:

        void Construct() override;


        FIELD()
        EditorViewport* viewport = nullptr;

    };
    
} // namespace CE::Editor

#include "ViewportWindow.rtti.h"