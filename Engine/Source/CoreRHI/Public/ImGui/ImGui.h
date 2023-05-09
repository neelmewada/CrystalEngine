#pragma once

#include "ImGuiDefinitions.h"

namespace CE
{
    
    class CORERHI_API IImGuiRenderer
    {
    public:

        virtual void InitImGui() {}

        virtual void ShutdownImGui() {}

    };
    
} // namespace CE

