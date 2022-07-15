
#define SDL_MAIN_HANDLED
#include "Engine.h"

#include <iostream>
#include <chrono>
#include <filesystem>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#if PLATFORM_WIN32
#include <Windows.h>
#endif

namespace fs = std::filesystem;

using namespace Vox;

Uint64 GetAlignedMemorySize(Uint64 originalSize, Uint64 alignment)
{
    return (originalSize + alignment - 1) & ~(alignment - 1);
}

class Application : public ApplicationBase
{
public:
    Application(std::string title) : ApplicationBase(title, false, false, 1280, 720)
    {

    }

    ~Application()
    {
        m_pDeviceContext->WaitUntilIdle(); // Wait until device is idle before destroying everything

        delete m_IndexBuffer;
        delete m_VertexBuffer;
        if (m_ObjectUniformData != nullptr)
            free(m_ObjectUniformData);
        delete m_pPSO;
        delete m_ObjectUniformBuffer;
        delete m_GlobalUniformBuffer;
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
        ApplicationBase::Start(); // call base function first

        auto* pEngineFactoryVk = EngineFactoryVk::Get();

        // -- Engine Creation --
        EngineCreateInfoVk engineInfo = {};
        engineInfo.applicationVersion = MAKE_VERSION(0, 0, 0, 1);
        engineInfo.applicationName = "Vox Sandbox";
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

        // -- View Projection Matrix --
        uint32_t w = 0, h = 0;
        m_pSwapChain->GetSize(&w, &h);

        m_GlobalUniforms.projection = glm::perspective(glm::radians(60.0f), (float)w / (float)h, 0.1f, 100.0f);
        m_GlobalUniforms.projection[1][1] *= -1;
        m_GlobalUniforms.view = glm::lookAt(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        // -- Model Matrix --
        for (int i = 0; i < _countof(m_ObjectUniform); ++i)
        {
            m_ObjectUniform[i].model = glm::mat4(1.0f);
        }

        // -- Render Context Creation --
        RenderContextCreateInfoVk renderContextInfo = {};
        renderContextInfo.engineContext = m_pEngineContext;
        renderContextInfo.deviceContext = m_pDeviceContext;
        renderContextInfo.swapChain = m_pSwapChain;
        pEngineFactoryVk->CreateRenderContextVk(renderContextInfo, &m_pRenderContext);

        // -- Global Uniform Buffer --
        BufferCreateInfo globalUniformBufferInfo = {};
        globalUniformBufferInfo.pName = "Global Uniforms";
        globalUniformBufferInfo.size = sizeof(GlobalUniforms);
        globalUniformBufferInfo.bindFlags = Vox::BIND_UNIFORM_BUFFER;
        globalUniformBufferInfo.allocType = Vox::BUFFER_MEM_CPU_TO_GPU; // CPU & GPU Shared Memory
        globalUniformBufferInfo.optimizationFlags = Vox::BUFFER_OPTIMIZE_UPDATE_SELDOM_BIT;

        BufferData globalUniformsData = {};
        globalUniformsData.dataSize = sizeof(GlobalUniforms);
        globalUniformsData.pData = &m_GlobalUniforms;

        m_GlobalUniformBuffer = m_pDeviceContext->CreateBuffer(globalUniformBufferInfo, globalUniformsData);

        // -- Object Uniform Buffer --
        Uint64 minBufferOffsetAlignment = m_pDeviceContext->GetMinUniformBufferOffsetAlignment();
        Uint64 stride = (sizeof(ObjectUniforms) + minBufferOffsetAlignment - 1) & ~(minBufferOffsetAlignment - 1);
        m_ObjectUniformDataSize = stride * _countof(m_ObjectUniform);
//        m_ObjectUniformData = (ObjectUniforms*)std::aligned_alloc(stride, m_ObjectUniformDataSize); // 4 elements
//        for (int i = 0; i < _countof(m_ObjectUniform); ++i)
//        {
//            auto ptr = (ObjectUniforms*)((Uint64)m_ObjectUniformData + stride * i);
//            *ptr = m_ObjectUniform[i];
//        }

        BufferCreateInfo objectBufferInfo = {};
        objectBufferInfo.pName = "Object Buffer";
        objectBufferInfo.size = m_ObjectUniformDataSize;
        objectBufferInfo.bindFlags = Vox::BIND_UNIFORM_BUFFER;
        objectBufferInfo.allocType = Vox::BUFFER_MEM_CPU_TO_GPU;
        objectBufferInfo.optimizationFlags = Vox::BUFFER_OPTIMIZE_UPDATE_REGULAR_BIT;
        objectBufferInfo.usageFlags = Vox::BUFFER_USAGE_DYNAMIC_OFFSET_BIT;
        objectBufferInfo.structureByteStride = stride;

        BufferData objectBufferData = {};
        objectBufferData.dataSize = m_ObjectUniformDataSize;
        objectBufferData.pData = m_ObjectUniformData;
        objectBufferData.offsetInBuffer = 0;

        //m_ObjectUniformBuffer = m_pDeviceContext->CreateBuffer(objectBufferInfo, objectBufferData);

        // -- Shaders --
        fs::path shaderDir = IO::FileManager::GetSharedDirectory();
        shaderDir = shaderDir / "shaders/";
        //shaderDir = std::string("/Users/neelmewada/CLionProjects/VoxEngine/RunnerTest/shaders");

        std::string vertexFile = (shaderDir / "shader2_vert.spv").string();
        std::string fragFile = (shaderDir / "shader2_frag.spv").string();

        std::vector<char> vertSpv, fragSpv;
        IO::ReadAllBytesFromFile(vertexFile, vertSpv);
        IO::ReadAllBytesFromFile(fragFile, fragSpv);

        // Shader Variant Creation
        ShaderVariantCreateInfo variant = {};
        variant.defineFlagsCount = 0; // No define flags used
        variant.pDefineFlags = nullptr;
        variant.vertexByteSize = vertSpv.size();
        variant.pVertexBytes = reinterpret_cast<const uint32_t*>(vertSpv.data());
        variant.fragmentByteSize = fragSpv.size();
        variant.pFragmentBytes = reinterpret_cast<const uint32_t*>(fragSpv.data());
        
        // Shader Creation (Collection of Variants)
        ShaderCreateInfo shaderInfo = {};
        shaderInfo.variantCount = 1;
        shaderInfo.pVariants = &variant;
        shaderInfo.pVertEntryPoint = "main";
        shaderInfo.pFragEntryPoint = "main";
        shaderInfo.defaultVariant = 0;

        IShader* shader = m_pDeviceContext->CreateShader(shaderInfo);

        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 color;
        };

        // -- Graphics Pipeline --
        GraphicsPipelineVertexAttributeDesc vertAttribs[2] = {
                {0, 0, static_cast<uint32_t>(offsetof(Vertex, position)), VERTEX_ATTRIB_FLOAT3}, // Position (float3)
                {0, 1, static_cast<uint32_t>(offsetof(Vertex, color)), VERTEX_ATTRIB_FLOAT3}  // Color    (float3)
        };

        ShaderResourceVariableDesc variables[2] = {
            {"GlobalUniformBuffer", Vox::SHADER_STAGE_ALL, SHADER_RESOURCE_VARIABLE_STATIC_BINDING_BIT},
            {"ObjectBuffer", Vox::SHADER_STAGE_ALL, static_cast<ShaderResourceVariableFlags>(
                                                            SHADER_RESOURCE_VARIABLE_STATIC_BINDING_BIT |
                                                            SHADER_RESOURCE_VARIABLE_DYNAMIC_OFFSET_BIT)}
        };

        GraphicsPipelineStateCreateInfo pipelineInfo = {};
        pipelineInfo.pDevice = m_pDeviceContext;
        pipelineInfo.pSwapChain = m_pSwapChain;
        pipelineInfo.pRenderContext = m_pRenderContext;
        pipelineInfo.pShader = shader;
        pipelineInfo.attributesCount = _countof(vertAttribs);
        pipelineInfo.pAttributes = vertAttribs;
        pipelineInfo.vertexStructByteSize = static_cast<uint32_t>(sizeof(Vertex));
        pipelineInfo.variableCount = _countof(variables);
        pipelineInfo.pResourceVariables = variables;
        m_pPSO = m_pDeviceContext->CreateGraphicsPipelineState(pipelineInfo);

        m_pPSO->GetStaticVariableByName("GlobalUniformBuffer")->Set(m_GlobalUniformBuffer);
        //m_pPSO->GetStaticVariableByName("ObjectBuffer")->Set(m_ObjectUniformBuffer);

        // -- MESH --

        Vertex vertices[4];
        // Vertex positions
        vertices[0].position = { 0.5f, 0.5f, 0.0f };
        vertices[1].position = {-0.5f, 0.5f, 0.0f };
        vertices[2].position = { -0.5f,-0.5f, 0.0f };
        vertices[3].position = { 0.5f,-0.5f, 0.0f };
        // Vertex colors
        vertices[0].color = { 0.0f, 1.0f, 0.0f };
        vertices[1].color = { 0.0f, 1.0f, 0.0f };
        vertices[2].color = { 0.0f, 1.0f, 0.0f };
        vertices[3].color = { 0.0f, 1.0f, 0.0f };

        uint16_t indices[6] = {0, 1, 2, 2, 3, 0};

        // Vertex Buffer
        BufferCreateInfo vertBufferInfo = {};
        vertBufferInfo.pName = "Rect";
        vertBufferInfo.bindFlags = Vox::BIND_VERTEX_BUFFER;
        vertBufferInfo.allocType = Vox::BUFFER_MEM_CPU_TO_GPU;
        vertBufferInfo.size = sizeof(vertices);
        BufferData vertBufferData = {};
        vertBufferData.dataSize = sizeof(vertices);
        vertBufferData.pData = vertices;
        m_VertexBuffer = m_pDeviceContext->CreateBuffer(vertBufferInfo, vertBufferData);

        // Index Buffer
        BufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.pName = "Rect";
        indexBufferInfo.bindFlags = Vox::BIND_INDEX_BUFFER;
        indexBufferInfo.allocType = Vox::BUFFER_MEM_CPU_TO_GPU;
        indexBufferInfo.size = sizeof(indices);
        BufferData indexBufferData = {};
        indexBufferData.dataSize = sizeof(indices);
        indexBufferData.pData = indices;
        m_IndexBuffer = m_pDeviceContext->CreateBuffer(indexBufferInfo, indexBufferData);

        /*
        // Uniform Buffer - OLD CODE
        Uint64 minBufferOffsetAlignment = m_pDeviceContext->GetMinUniformBufferOffsetAlignment();
        modelBufferAlignment = (sizeof(ObjectUniforms) + minBufferOffsetAlignment - 1) & ~(minBufferOffsetAlignment - 1);
        m_ObjectUniformData = (ObjectUniforms*)std::aligned_alloc(modelBufferAlignment, modelBufferAlignment * 1);

        *m_ObjectUniformData = m_ObjectUniform;
        BufferData modelData{};
        modelData.offsetInBuffer = 0;
        modelData.dataSize = modelBufferAlignment;
        modelData.pData = m_ObjectUniformData;

        BufferData vpData{};
        vpData.dataSize = sizeof(m_GlobalUniforms);
        vpData.pData = &m_GlobalUniforms;

        // OLD CODE
        m_pPSO->CreateUniformBuffer(sizeof(UboViewProjection), vpData);
        m_pPSO->CreateDynamicUniformBuffer(modelBufferAlignment, modelData, minBufferOffsetAlignment);
        */

        // -- Record Commands --
        /*float clearColor[4] = {0.6f, 0.65f, 0.4f, 1.0f};

        m_RenderContext->SetClearColor(clearColor);

        m_RenderContext->Begin();
        m_RenderContext->CmdBindPipeline(m_pPSO);
        uint64_t offsetInBuffer = 0;
        // This function expects elements of pBuffers to not be destroyed, so it can be called in ReRecordCommands
        m_RenderContext->CmdBindVertexBuffers(1, &m_VertexBuffer, &offsetInBuffer);
        m_RenderContext->CmdBindIndexBuffer(m_IndexBuffer, INDEX_TYPE_UINT16, 0);
        m_RenderContext->CmdDrawIndexed(6, 1, 0, 0);
        m_RenderContext->End();*/

        // We're responsible for deleting the shader coz we created it ourselves.
        delete shader;
    }

protected:
    void PollEvent(SDL_Event e) override
    {
        // Handle OS events ...
        if (e.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            m_pSwapChain->Resize();

            uint32_t w = 0, h = 0;
            m_pSwapChain->GetSize(&w, &h);

            m_GlobalUniforms.projection = glm::perspective(glm::radians(60.0f), (float)w / (float)h, 0.1f, 100.0f);
            m_GlobalUniforms.projection[1][1] *= -1;

            BufferData globalUniformData = {};
            globalUniformData.dataSize = sizeof(m_GlobalUniforms);
            globalUniformData.pData = &m_GlobalUniforms;
            m_GlobalUniformBuffer->SetBufferData(globalUniformData);

            //m_RenderContext->UpdateGlobalUniforms(m_GlobalUniforms);

            //BufferData uniformData;
            //uniformData.dataSize = sizeof(m_GlobalUniforms);
            //uniformData.pData = &m_GlobalUniforms;
            //m_pPSO->UpdateUniformBuffer(uniformData);
        }
    }

    void Render() override
    {
        float clearColor[4] = {0.6f, 0.65f, 0.4f, 1.0f};

        m_pRenderContext->SetClearColor(clearColor);

        m_pRenderContext->Begin();
        m_pRenderContext->CmdBindPipeline(m_pPSO);
        uint64_t offset = 0;
        // This function expects elements of pBuffers to not be destroyed, so it can be called in ReRecordCommands
        m_pRenderContext->CmdBindVertexBuffers(1, &m_VertexBuffer, &offset);
        m_pRenderContext->CmdBindIndexBuffer(m_IndexBuffer, INDEX_TYPE_UINT16, 0);
        m_pRenderContext->CmdDrawIndexed(6, 1, 0, 0, 0);
        m_pRenderContext->End();

        m_pSwapChain->Submit();
        m_pSwapChain->Present();
    }

    void Update() override
    {
        static float angle = 0.0f;
        angle += m_DeltaTime * 30;
        if (angle > 360) angle -= 360;

        m_ObjectUniform[0].model = glm::mat4(1.0f) * glm::rotate(glm::radians(angle), glm::vec3(0, 1, 0));

//        m_ObjectUniformData->m_ObjectUniform = glm::mat4(1.0f) * glm::rotate(glm::radians(angle), glm::vec3(0, 1, 0));
//
//        BufferData modelData{};
//        modelData.offsetInBuffer = 0;
//        modelData.dataSize = modelBufferAlignment;
//        modelData.pData = m_ObjectUniformData;
//
//        m_pPSO->UpdateDynamicUniformBuffer(modelData);

    }

private: // Vulkan Functions


private: // Internal Members
    IEngineContext* m_pEngineContext;
    IDeviceContext* m_pDeviceContext;
    ISwapChain* m_pSwapChain;
    IRenderContext* m_pRenderContext;
    IGraphicsPipelineState* m_pPSO;
    IBuffer* m_VertexBuffer;
    IBuffer* m_IndexBuffer;
    IBuffer* m_GlobalUniformBuffer;
    IBuffer* m_ObjectUniformBuffer;

    struct GlobalUniforms
    {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
    } m_GlobalUniforms;

    struct ObjectUniforms {
        alignas(16) glm::vec3 colorTint;
        alignas(16) glm::mat4 model;
    } m_ObjectUniform[4];

    ObjectUniforms* m_ObjectUniformData = nullptr;
    size_t m_ObjectUniformDataSize;
};


int main(int argc, char* argv[])
{
    Application app("VoxEngine Test");
    app.Start();
    app.Run();
    return 0;
}

#ifdef PLATFORM_WIN32
int WINAPI CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}
#endif

