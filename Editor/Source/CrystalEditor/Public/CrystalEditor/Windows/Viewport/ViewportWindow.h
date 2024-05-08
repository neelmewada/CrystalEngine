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


        
    };
    
} // namespace CE::Editor

#include "ViewportWindow.rtti.h"