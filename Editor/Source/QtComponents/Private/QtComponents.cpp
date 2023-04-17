
#include "QtComponents.h"

#include "QtComponents.private.h"
CE_IMPLEMENT_MODULE_AUTORTTI(QtComponents, CE::Editor::Qt::QtComponentsModule)

namespace CE::Editor::Qt
{

    void QtComponentsModule::StartupModule()
    {
        
    }

    void QtComponentsModule::ShutdownModule()
    {
        
    }

    void QtComponentsModule::RegisterTypes()
    {
        // Input Fields
        CE_REGISTER_TYPES(
            Vec4Input,
            StringField,
            EnumField
        );
    }
    
} // namespace CE::Editor::Qt
