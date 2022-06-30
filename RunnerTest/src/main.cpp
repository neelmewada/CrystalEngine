
#include "Engine.h"

#include <iostream>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

using namespace Vox;


class Application : public ApplicationBase
{
public:
    Application(std::string title) : ApplicationBase(title)
    {

    }

    ~Application()
    {
        delete m_pRenderContext;
        delete m_pSwapChain;
        delete m_pDeviceContext;
        delete m_pEngineContext;
    }

public:
    const char * GetApplicationName() override { return "VoxEngine Test App"; }
    uint32_t GetApplicationVersion() override { return MAKE_VERSION(0, 0, 1, 0); }

    virtual void Start() override
    {
        auto* pEngineFactoryVk = EngineFactoryVk::Get();

        // -- Engine Creation --
        EngineCreateInfoVk engineInfo = {};
        engineInfo.applicationVersion = MAKE_VERSION(0, 0, 0, 1);
        engineInfo.applicationName = "Vox Test";
#if NDEBUG
        engineInfo.enableValidationLayers = false;
#else
        engineInfo.enableValidationLayers = true;
#endif
        pEngineFactoryVk->CreateEngineContextVk(engineInfo, &m_pEngineContext);

        // -- Device Creation --
        DeviceCreateInfoVk deviceInfo = {};
        deviceInfo.engineContext = m_pEngineContext;
        pEngineFactoryVk->CreateDeviceContextVk(deviceInfo, &m_pDeviceContext);

        // -- SwapChain Creation --
        SwapChainCreateInfoVk swapChainInfo = {};
        swapChainInfo.engineContext = m_pEngineContext;
        swapChainInfo.deviceContext = m_pDeviceContext;
        pEngineFactoryVk->CreateSwapChainVk(swapChainInfo, &m_pSwapChain);

        // -- Render Context Creation --
        RenderContextCreateInfoVk renderContextInfo = {};
        renderContextInfo.engineContext = m_pEngineContext;
        renderContextInfo.deviceContext = m_pDeviceContext;
        renderContextInfo.swapChain = m_pSwapChain;
        pEngineFactoryVk->CreateRenderContextVk(renderContextInfo, &m_pRenderContext);

        m_pRenderContext->SetClearColor((float[4]){0.6f, 0.65f, 0.4f, 1.0f});

        m_pRenderContext->BeginRecording();
        //m_pRenderContext->CmdBindPipeline(cubePipeline);
        //m_pRenderContext->CmdBindVertexBuffers(2, vertexBuffers);
        //m_pRenderContext->CmdBindIndexBuffer(indexBuffers);
        //m_pRenderContext->CmdDrawIndexed();
        //m_pRenderContext->EndRecording();

        fs::path shaderDir = IO::GetSharedDirectory();
        shaderDir = shaderDir / "shaders/";

        std::string vertexFile = (shaderDir / "shader_vert.spv").string();
        std::string fragFile = (shaderDir / "shader_frag.spv").string();

        std::vector<char> vertSpv, fragSpv;
        IO::ReadAllBytesFromFile(vertexFile, vertSpv);
        IO::ReadAllBytesFromFile(fragFile, fragSpv);

        ShaderVariantCreateInfo variant = {};
        variant.defineFlagsCount = 0; // No define flags used
        variant.pDefineFlags = nullptr;
        variant.vertexByteSize = vertSpv.size();
        variant.pVertexBytes = reinterpret_cast<const uint32_t*>(vertSpv.data());
        variant.fragmentByteSize = fragSpv.size();
        variant.pFragmentBytes = reinterpret_cast<const uint32_t*>(fragSpv.data());

        ShaderCreateInfo shaderInfo = {};
        shaderInfo.variantCount = 1;
        shaderInfo.pShaderVariants = &variant;

        IShader* shader = m_pDeviceContext->CreateShader(shaderInfo);

        GraphicsPipelineStateCreateInfo pipelineInfo = {};
        pipelineInfo.device = m_pDeviceContext;
        //pipelineInfo.shader = shader;

        //m_pPSO = m_pDeviceContext->CreateGraphicsPipelineState(pipelineInfo);

        delete shader;
    }

protected:
    void PollEvent(SDL_Event e) override
    {

    }

    void Render() override
    {
        m_pSwapChain->Submit();
        m_pSwapChain->Present();
    }

    void Update() override
    {

    }

private: // Vulkan Functions


private: // Internal Members
    IEngineContext* m_pEngineContext;
    IDeviceContext* m_pDeviceContext;
    ISwapChain* m_pSwapChain;
    IRenderContext* m_pRenderContext;
    IGraphicsPipelineState* m_pPSO;
};

int main(int argc, const char* argv[])
{
    Application app("VoxEngine Test");
    app.Start();
    app.Run();
    return 0;
}
