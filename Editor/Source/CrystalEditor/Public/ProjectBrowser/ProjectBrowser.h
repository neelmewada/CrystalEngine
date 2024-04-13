#pragma once

namespace CE::Editor
{

    CLASS()
    class CRYSTALEDITOR_API ProjectBrowser : public CToolWindow
    {
        CE_CLASS(ProjectBrowser, CToolWindow)
    public:

        ProjectBrowser();

        virtual ~ProjectBrowser();

    protected:

        void Construct() override;

        FUNCTION()
        void OnProjectTemplateSelectionChanged();

        
    };
    
} // namespace CE::Editor

#include "ProjectBrowser.rtti.h"