
#include "Core.h"

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
        // Register Data types
        CE_REGISTER_TYPES(bool, CE::s8, CE::s16, CE::s32, CE::s64,
            CE::u8, CE::u16, CE::u32, CE::u64, CE::f32, CE::f64,
            CE::String, CE::UUID, CE::Name,
            CE::Vec2, CE::Vec3, CE::Vec4, CE::Vec2i, CE::Vec3i, CE::Vec4i,
            CE::Quat, CE::Matrix4x4);

        CE_REGISTER_TYPES(
            ProjectSettings
        );

        // Enums
        CE_REGISTER_TYPES(
            EventResult
        );

        // Register Objects
        CE_REGISTER_TYPES(
            Object,
            SystemObject,
            Component,
            SystemComponent
        );
    }

}
