#pragma once

#include "ImGuiDefinitions.h"

namespace CE
{
    
    class CORERHI_API IImGuiRenderer
    {
    public:

        virtual ~IImGuiRenderer() {}

        virtual bool InitImGui(IMGUIFontPreloadConfig* preloadFonts = nullptr) = 0;

        virtual void ShutdownImGui() = 0;

    };
    
} // namespace CE

