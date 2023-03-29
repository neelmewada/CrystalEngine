#pragma once

#include "Module/ModuleManager.h"

#include "CEQtApplication.h"
#include "Misc/QStringFmt.h"

#include "Project/EditorProjectSettings.h"
#include "Project/ProjectManager.h"

// Drawers
#include "Drawers/DrawerBase.h"
#include "Drawers/FieldDrawer.h"
#include "Drawers/StringFieldDrawer.h"
#include "Drawers/VectorFieldDrawer.h"
#include "Drawers/EnumFieldDrawer.h"

// Asset Import Settings
#include "Asset/AssetImportSettings.h"
#include "Asset/TextureAssetImportSettings.h"
#include "Asset/AssetManager.h"

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
