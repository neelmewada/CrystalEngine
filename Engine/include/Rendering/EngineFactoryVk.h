#pragma once

#include "EngineDefs.h"

#include "IBuffer.h"
#include "Rendering/IEngineContext.h"
#include "Rendering/IDeviceContext.h"
#include "Rendering/ISwapChain.h"
#include "Rendering/IRenderContext.h"

#include <stdint.h>

namespace Vox
{

struct EngineCreateInfoVk
{
    const char* applicationName;
    uint32_t applicationVersion;
    bool enableValidationLayers;
};

struct DeviceCreateInfoVk
{
    IEngineContext* engineContext;
};

struct SwapChainCreateInfoVk
{
    IEngineContext* engineContext;
    IDeviceContext* deviceContext;
};

struct RenderContextCreateInfoVk
{
    IEngineContext* engineContext;
    IDeviceContext* deviceContext;
    ISwapChain* swapChain;
    //GlobalUniforms initialGlobalUniforms;
};

class ENGINE_API EngineFactoryVk
{
private:
    EngineFactoryVk();

public:
    DELETE_COPY_CONSTRUCTORS(EngineFactoryVk)

public:
    static EngineFactoryVk* Get() { return m_Instance; }

public: // Public API
    void CreateEngineContextVk(EngineCreateInfoVk& engineInfo, IEngineContext **ppEngineContext);
    void CreateDeviceContextVk(DeviceCreateInfoVk& deviceInfo, IDeviceContext **ppDeviceContext);
    void CreateSwapChainVk(SwapChainCreateInfoVk& swapChainInfo, ISwapChain **ppSwapChain);
    void CreateRenderContextVk(RenderContextCreateInfoVk& renderContextInfo, IRenderContext **ppRenderContext);

private:
    static EngineFactoryVk* m_Instance;
};

}

