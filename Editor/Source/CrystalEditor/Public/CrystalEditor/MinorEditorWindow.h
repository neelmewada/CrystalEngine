#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class CRYSTALEDITOR_API MinorEditorWindow : public CDockWindow
    {
        CE_CLASS(MinorEditorWindow, CDockWindow)
    public:

        MinorEditorWindow();
        virtual ~MinorEditorWindow();

    protected:

        void Construct() override;

    };
    
} // namespace CE::Editor

#include "MinorEditorWindow.rtti.h"