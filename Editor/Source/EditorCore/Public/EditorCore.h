#pragma once

#include "Module/ModuleManager.h"

#include "CEQtApplication.h"
#include "Misc/QStringFmt.h"

#include "Project/ProjectManager.h"

namespace CE::Editor
{
    
    class EDITORCORE_API EditorCoreModule : public Module
    {
    public:

        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;

    };

} // namespace CE::Editor
