
#include "QtComponents.h"

CE_IMPLEMENT_MODULE(QtComponents, CE::Editor::Qt::QtComponentsModule)

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
