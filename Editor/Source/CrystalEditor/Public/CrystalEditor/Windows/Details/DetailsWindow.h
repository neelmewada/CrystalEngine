#pragma once

namespace CE::Editor
{

    CLASS()
    class CRYSTALEDITOR_API DetailsWindow final : public MinorEditorWindow
    {
        CE_CLASS(DetailsWindow, MinorEditorWindow)
    public:

        DetailsWindow();

        virtual ~DetailsWindow();

    private:

        void Construct() override;

    };
    
} // namespace CE::Editor

#include "DetailsWindow.rtti.h"