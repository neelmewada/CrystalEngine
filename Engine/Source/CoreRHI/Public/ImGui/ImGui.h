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

        virtual void ImGuiNewFrame() = 0;

        virtual void ImGuiRender() = 0;

        virtual void ImGuiPlatformUpdate() = 0;
    };
    
} // namespace CE

