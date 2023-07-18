
#include "CoreMedia.h"


CE_IMPLEMENT_MODULE(CoreMedia, CE::CoreMediaModule)

#define STBI_FAILURE_USERMSG

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

namespace CE
{
    void CoreMediaModule::StartupModule()
    {

    }

    void CoreMediaModule::ShutdownModule()
    {

    }

    void CoreMediaModule::RegisterTypes()
    {

    }
}
