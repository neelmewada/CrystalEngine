#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API ProjectBrowser : public CE::Widgets::CToolWindow
    {
        CE_CLASS(ProjectBrowser, CE::Widgets::CToolWindow)
    public:

        ProjectBrowser();
        virtual ~ProjectBrowser();

    protected:

    };
    
} // namespace CE::Editor

#include "ProjectBrowser.rtti.h"