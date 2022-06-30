#pragma once

#include "Rendering/EngineFactoryVk.h"
#include "Rendering/IRenderContext.h"

#include "SwapChainVk.h"

#include "EngineContextVk.h"
#include "DeviceContextVk.h"
#include "SwapChainVk.h"

namespace Vox
{

class RenderContextVk : public IRenderContext
{
private:
    friend class EngineFactoryVk;
    RenderContextVk(RenderContextCreateInfoVk& renderContextInfo);
    ~RenderContextVk();

public: // Public API
    // - Setup
    void SetClearColor(float clearColor[4]) override;

    // - Commands
    void BeginRecording() override;
    void EndRecording() override;

private: // Internal API

private: // Internal Members
    // - References
    EngineContextVk* m_pEngine;
    DeviceContextVk* m_pDevice;
    SwapChainVk* m_pSwapChain;

    // - Info
    float m_ClearColor[4] = {0,0,0,0};

private: // Vulkan Members

};

}
