#pragma once

#include "Rendering/EngineFactoryVk.h"
#include "Rendering/IEngineContext.h"

#include <SDL2/SDL.h>

#include <stdint.h>
#include <vector>

#include <vulkan/vulkan.h>

namespace Vox
{

extern const std::vector<const char *> k_ValidationLayers;
extern const std::vector<const char *> k_RequiredDeviceExtensions;

class EngineContextVk : public IEngineContext
{
private:
    friend class EngineFactoryVk;
    EngineContextVk(EngineCreateInfoVk& engineInfo);
    ~EngineContextVk();

public:
    EngineContextVk(const EngineContextVk&) = delete;

public: // Public API
    // - Getters
    SDL_Window* GetWindow() { return m_Window; }
    VkInstance GetInstance() { return m_Instance; }

private: // Internal API
    bool CheckInstanceExtensionSupport(std::vector<const char*>& checkExtensions);

private: // Internal Members
    bool m_ValidationLayersEnabled = false;
    SDL_Window* m_Window = nullptr;

private: // Vulkan Members
    VkInstance m_Instance = nullptr;
    VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;

};

}
