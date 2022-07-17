
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
        delete m_pPerFrameSRB;
        delete m_pStaticSRB;
        delete m_pPSO;
        delete m_Texture2;
        delete m_Texture;
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

protected:
    void Start() override
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
        globalUniformBufferInfo.allocType = Vox::DEVICE_MEM_CPU_TO_GPU; // CPU & GPU Shared Memory
        globalUniformBufferInfo.optimizationFlags = Vox::BUFFER_OPTIMIZE_UPDATE_SELDOM_BIT;

        BufferData globalUniformsData = {};
        globalUniformsData.dataSize = sizeof(GlobalUniforms);
        globalUniformsData.pData = &m_GlobalUniforms;

        m_GlobalUniformBuffer = m_pDeviceContext->CreateBuffer(globalUniformBufferInfo, globalUniformsData);

        // -- Object Uniform Buffer --
        Uint64 minBufferOffsetAlignment = m_pDeviceContext->GetMinUniformBufferOffsetAlignment();
        m_ObjectUniformStride = (sizeof(ObjectUniforms) + minBufferOffsetAlignment - 1) & ~(minBufferOffsetAlignment - 1);
        m_ObjectUniformDataSize = m_ObjectUniformStride * _countof(m_ObjectUniform);
        m_ObjectUniformData = (ObjectUniforms*)malloc(m_ObjectUniformDataSize);
        for (int i = 0; i < _countof(m_ObjectUniform); ++i)
        {
            auto ptr = (ObjectUniforms*)((Uint64)m_ObjectUniformData + m_ObjectUniformStride * i);
            *ptr = m_ObjectUniform[i];
        }

        BufferCreateInfo objectBufferInfo = {};
        objectBufferInfo.pName = "Object Buffer";
        objectBufferInfo.size = m_ObjectUniformDataSize;
        objectBufferInfo.bindFlags = Vox::BIND_UNIFORM_BUFFER;
        objectBufferInfo.allocType = Vox::DEVICE_MEM_CPU_TO_GPU;
        objectBufferInfo.optimizationFlags = Vox::BUFFER_OPTIMIZE_UPDATE_REGULAR_BIT;
        objectBufferInfo.usageFlags = Vox::BUFFER_USAGE_DYNAMIC_OFFSET_BIT;
        objectBufferInfo.structureByteStride = m_ObjectUniformStride;

        BufferData objectBufferData = {};
        objectBufferData.dataSize = m_ObjectUniformDataSize;
        objectBufferData.pData = m_ObjectUniformData;
        objectBufferData.offsetInBuffer = 0;

        m_ObjectUniformBuffer = m_pDeviceContext->CreateBuffer(objectBufferInfo, objectBufferData);

        // -- Load Texture From File --
        fs::path binDir = IO::FileManager::GetBinDirectory();
        std::vector<char> textureData;
#if PLATFORM_MACOS
        IO::ReadAllBytesFromFile(binDir / "../../../textures/crate.png", textureData);
#else
        IO::ReadAllBytesFromFile((binDir / "textures/crate.png").string(), textureData);
#endif

        // -- Texture Samplers --
        TextureCreateInfo textureInfo = {};
        textureInfo.pName = "Crate 1";
        textureInfo.imageDataSize = textureData.size();
        textureInfo.pImageData = textureData.data();
        m_Texture = m_pDeviceContext->CreateTexture(textureInfo);
        m_TextureView = m_Texture->GetDefaultView();

#if PLATFORM_MACOS
        IO::ReadAllBytesFromFile(binDir / "../../../textures/crate.png", textureData);
#else
        IO::ReadAllBytesFromFile((binDir / "textures/crate2.jpg").string(), textureData);
#endif
        textureInfo.pName = "Crate 2";
        textureInfo.imageDataSize = textureData.size();
        textureInfo.pImageData = textureData.data();

        m_Texture2 = m_pDeviceContext->CreateTexture(textureInfo);
        m_TextureView2 = m_Texture2->GetDefaultView();

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
            glm::vec2 uv;
        };

        // -- Graphics Pipeline --
        GraphicsPipelineVertexAttributeDesc vertAttribs[3] = {
                {0, 0, static_cast<Uint32>(offsetof(Vertex, position)), VERTEX_ATTRIB_FLOAT3}, // Position (float3)
                {0, 1, static_cast<Uint32>(offsetof(Vertex, color)), VERTEX_ATTRIB_FLOAT3},  // Color    (float3)
                {0, 2, static_cast<Uint32>(offsetof(Vertex, uv)), VERTEX_ATTRIB_FLOAT2},     // UV (float2)
        };

        ShaderResourceVariableDesc variables[3] = {
            {"GlobalUniformBuffer", SHADER_STAGE_ALL, SHADER_RESOURCE_VARIABLE_STATIC_BINDING_BIT},
            {"ObjectBuffer", SHADER_STAGE_ALL, static_cast<ShaderResourceVariableFlags>(
                                                            SHADER_RESOURCE_VARIABLE_STATIC_BINDING_BIT |
                                                            SHADER_RESOURCE_VARIABLE_DYNAMIC_OFFSET_BIT)},
            {"tex", SHADER_STAGE_ALL, SHADER_RESOURCE_VARIABLE_DYNAMIC_BINDING_BIT}
        };

        ImmutableSamplerDesc immutableSamplers[] = {
                {"tex", SHADER_STAGE_ALL, SAMPLER_FILTER_TYPE_LINEAR, SAMPLER_FILTER_TYPE_LINEAR, SAMPLER_FILTER_TYPE_LINEAR,
                 SAMPLER_ADDRESS_MODE_REPEAT, SAMPLER_ADDRESS_MODE_REPEAT, SAMPLER_ADDRESS_MODE_REPEAT, 16}
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
        pipelineInfo.immutableSamplersCount = _countof(immutableSamplers);
        pipelineInfo.pImmutableSamplers = immutableSamplers;
        m_pPSO = m_pDeviceContext->CreateGraphicsPipelineState(pipelineInfo);
        m_pStaticSRB = m_pPSO->CreateResourceBinding(RESOURCE_BINDING_FREQUENCY_STATIC);
        m_pPerFrameSRB = m_pPSO->CreateResourceBinding(RESOURCE_BINDING_FREQUENCY_PER_FRAME);

        m_pStaticSRB->GetVariableByName("GlobalUniformBuffer")->Set(m_GlobalUniformBuffer);
        m_pPerFrameSRB->GetVariableByName("ObjectBuffer")->Set(m_ObjectUniformBuffer);
        m_pPerFrameSRB->GetVariableByName("tex")->Set(m_TextureView);

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
        // Vertex UVs
        vertices[0].uv = {1.0f, 0.0f};
        vertices[1].uv = {0.0f, 0.0f};
        vertices[2].uv = {0.0f, 1.0f};
        vertices[3].uv = {1.0f, 1.0f};

        uint16_t indices[6] = {0, 1, 2, 2, 3, 0};

        // Vertex Buffer
        BufferCreateInfo vertBufferInfo = {};
        vertBufferInfo.pName = "Rect";
        vertBufferInfo.bindFlags = Vox::BIND_VERTEX_BUFFER;
        vertBufferInfo.allocType = Vox::DEVICE_MEM_CPU_TO_GPU;
        vertBufferInfo.size = sizeof(vertices);
        BufferData vertBufferData = {};
        vertBufferData.dataSize = sizeof(vertices);
        vertBufferData.pData = vertices;
        m_VertexBuffer = m_pDeviceContext->CreateBuffer(vertBufferInfo, vertBufferData);

        // Index Buffer
        BufferCreateInfo indexBufferInfo = {};
        indexBufferInfo.pName = "Rect";
        indexBufferInfo.bindFlags = Vox::BIND_INDEX_BUFFER;
        indexBufferInfo.allocType = Vox::DEVICE_MEM_CPU_TO_GPU;
        indexBufferInfo.size = sizeof(indices);
        BufferData indexBufferData = {};
        indexBufferData.dataSize = sizeof(indices);
        indexBufferData.pData = indices;
        m_IndexBuffer = m_pDeviceContext->CreateBuffer(indexBufferInfo, indexBufferData);

        // We're responsible for deleting the shader because we created it ourselves.
        delete shader;
    }

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

            //m_pRenderContext->UpdateGlobalUniforms(m_GlobalUniforms);

            //BufferData uniformData;
            //uniformData.dataSize = sizeof(m_GlobalUniforms);
            //uniformData.pData = &m_GlobalUniforms;
            //m_pPSO->UpdateUniformBuffer(uniformData);
        }

        static bool useTex2 = false;

        if (e.key.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
        {
            useTex2 = !useTex2;
            m_pPerFrameSRB->GetVariableByName("tex")->Set(useTex2 ? m_TextureView2 : m_TextureView);
        }
    }

    void Render() override
    {
        float clearColor[4] = {0.6f, 0.65f, 0.4f, 1.0f};

        m_pRenderContext->SetClearColor(clearColor);

        // Begin Render Pass
        m_pRenderContext->Begin();

        // Bind Graphics Pipeline
        m_pRenderContext->CmdBindGraphicsPipeline(m_pPSO);
        m_pRenderContext->CmdBindShaderResources(m_pStaticSRB);
        m_pRenderContext->CmdBindShaderResources(m_pPerFrameSRB);

        // TODO: API to Bind Descriptor Sets (IShaderResourceBinding* object) (For later)
        //m_pRenderContext->CmdBindShaderResource(m_pStaticShaderResource);

        // Bind Mesh Data
        uint64_t offset = 0;
        m_pRenderContext->CmdBindVertexBuffers(1, &m_VertexBuffer, &offset);
        m_pRenderContext->CmdBindIndexBuffer(m_IndexBuffer, INDEX_TYPE_UINT16, 0);
        m_pRenderContext->CmdDrawIndexed(6, 1, 0, 0, 0);

        // End Render Pass
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

        for (int i = 0; i < _countof(m_ObjectUniform); ++i)
        {
            auto ptr = (ObjectUniforms*)((Uint64)m_ObjectUniformData + m_ObjectUniformStride * i);
            *ptr = m_ObjectUniform[i];
        }

        BufferData objectUniformData = {};
        objectUniformData.dataSize = m_ObjectUniformDataSize;
        objectUniformData.pData = m_ObjectUniformData;

        m_ObjectUniformBuffer->SetBufferData(objectUniformData);
    }

private: // Vulkan Functions


private: // Internal Members
    IEngineContext* m_pEngineContext;
    IDeviceContext* m_pDeviceContext;
    ISwapChain* m_pSwapChain;
    IRenderContext* m_pRenderContext;
    IGraphicsPipelineState* m_pPSO;
    IShaderResourceBinding* m_pStaticSRB;
    IShaderResourceBinding* m_pPerFrameSRB;
    IBuffer* m_VertexBuffer;
    IBuffer* m_IndexBuffer;
    IBuffer* m_GlobalUniformBuffer;
    IBuffer* m_ObjectUniformBuffer;
    ITexture* m_Texture;
    ITextureView* m_TextureView;
    ITexture* m_Texture2;
    ITextureView* m_TextureView2;

    struct GlobalUniforms
    {
        alignas(16) glm::mat4 projection;
        alignas(16) glm::mat4 view;
    } m_GlobalUniforms;

    struct ObjectUniforms {
        alignas(16) glm::mat4 model;
        alignas(16) glm::vec3 colorTint;
    } m_ObjectUniform[4];

    ObjectUniforms* m_ObjectUniformData = nullptr;
    size_t m_ObjectUniformDataSize;
    size_t m_ObjectUniformStride;
};


int main(int argc, char* argv[])
{
    Application app("VoxEngine Test");
    app.Run();
    return 0;
}

#ifdef PLATFORM_WIN32
int WINAPI CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return main(__argc, __argv);
}
#endif

