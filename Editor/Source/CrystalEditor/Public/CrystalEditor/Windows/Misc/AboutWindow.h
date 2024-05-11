#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API AboutWindow final : public CToolWindow
    {
        CE_CLASS(AboutWindow, CToolWindow)
    public:

        AboutWindow();

        virtual ~AboutWindow();

    private:

        void Construct() override;

    };
    
} // namespace CE::Editor

#include "AboutWindow.rtti.h"