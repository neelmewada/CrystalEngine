
#include "Core.h"

#include "Object/Object.h"

CE_IMPLEMENT_MODULE(Core, CE::CoreModule)

namespace CE
{


    void CoreModule::StartupModule()
    {

    }

    void CoreModule::ShutdownModule()
    {

    }

    void CoreModule::RegisterTypes()
    {
        CE_REGISTER_TYPES(bool, CE::s8, CE::s16, CE::s32, CE::s64,
            CE::u8, CE::u16, CE::u32, CE::u64, CE::f32, CE::f64,
            CE::String);

        CE_REGISTER_TYPES(
            Object
        );
    }

}
