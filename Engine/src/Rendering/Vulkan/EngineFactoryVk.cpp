
#include "Rendering/EngineFactoryVk.h"
#include "EngineContextVk.h"
#include "DeviceContextVk.h"
#include "SwapChainVk.h"
#include "RenderContextVk.h"

#include <iostream>

using namespace Vox;

EngineFactoryVk* EngineFactoryVk::m_Instance = new EngineFactoryVk();

EngineFactoryVk::EngineFactoryVk()
{

}

void EngineFactoryVk::CreateEngineContextVk(EngineCreateInfoVk& engineInfo, IEngineContext **ppEngineContext)
{
    *ppEngineContext = new EngineContextVk(engineInfo);
}

void EngineFactoryVk::CreateDeviceContextVk(DeviceCreateInfoVk &deviceInfo, IDeviceContext **ppDeviceContext)
{
    *ppDeviceContext = new DeviceContextVk(deviceInfo);
}

void EngineFactoryVk::CreateSwapChainVk(SwapChainCreateInfoVk& swapChainInfo, ISwapChain **ppSwapChain)
{
    *ppSwapChain = new SwapChainVk(swapChainInfo);
}

void EngineFactoryVk::CreateRenderContextVk(RenderContextCreateInfoVk &renderContextInfo, IRenderContext **ppRenderContext)
{
    *ppRenderContext = new RenderContextVk(renderContextInfo);
}
