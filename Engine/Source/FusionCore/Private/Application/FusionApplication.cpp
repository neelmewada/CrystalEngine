#include "FusionCore.h"

namespace CE
{
    static FusionApplication* gInstance = nullptr;

    extern RawData GetFusionShaderVert();
    extern RawData GetFusionShaderFrag();
    extern RawData GetFusionShaderVertJson();
    extern RawData GetFusionShaderFragJson();

    FusionApplication::FusionApplication()
    {

    }

    FusionApplication::~FusionApplication()
    {
        if (gInstance == this)
        {
            gInstance = nullptr;
        }
    }

    FusionApplication* FusionApplication::Get()
    {
        if (gInstance == nullptr)
        {
            gInstance = CreateObject<FusionApplication>(GetTransientPackage(MODULE_NAME), "FusionApplication");
        }
        return gInstance;
    }

    FusionApplication* FusionApplication::TryGet()
    {
        return gInstance;
    }

    void FusionApplication::Initialize(const FusionInitInfo& initInfo)
    {
        InitializeShaders();
    }

    void FusionApplication::Shutdown()
    {
        delete fusionShader; fusionShader = nullptr;
    }

    void FusionApplication::Tick()
    {

    }

    void FusionApplication::InitializeShaders()
    {
        String basePath = "/" MODULE_NAME "/Resources/Shaders/FusionShader";

        RawData vertexShader = GetFusionShaderVert();
        RawData fragmentShader = GetFusionShaderFrag();

        String vertexShaderJson = (char*)GetFusionShaderVertJson().data;
        String fragmentShaderJson = (char*)GetFusionShaderFragJson().data;

        JValue vertexReflection{};
        JValue fragmentReflection{};

        JsonSerializer::Deserialize2(vertexShaderJson, vertexReflection);
        JsonSerializer::Deserialize2(fragmentShaderJson, fragmentReflection);

        RPI::ShaderVariantDescriptor2 variantDesc{};
        variantDesc.shaderName = "FusionShader";
        variantDesc.entryPoints.Resize(2);
        variantDesc.entryPoints[0] = "VertMain";
        variantDesc.entryPoints[1] = "FragMain";

        variantDesc.moduleDesc.Resize(2);
        variantDesc.moduleDesc[0].byteCode = vertexShader.data;
        variantDesc.moduleDesc[0].byteSize = vertexShader.dataSize;
        variantDesc.moduleDesc[0].stage = ShaderStage::Vertex;
        variantDesc.moduleDesc[0].name = "VertMain";

        variantDesc.moduleDesc[1].byteCode = fragmentShader.data;
        variantDesc.moduleDesc[1].byteSize = fragmentShader.dataSize;
        variantDesc.moduleDesc[1].stage = ShaderStage::Fragment;
        variantDesc.moduleDesc[1].name = "FragMain";

        RHI::SRGVariableDescriptor perViewData{};
        perViewData.name = "_PerViewData";
        perViewData.bindingSlot = (u32)vertexReflection["ubos"][0]["binding"].GetNumberValue();
        perViewData.shaderStages = ShaderStage::Vertex | ShaderStage::Fragment;
        perViewData.type = ShaderResourceType::ConstantBuffer;

        variantDesc.reflectionInfo.FindOrAdd(SRGType::PerView)
            .TryAdd(perViewData);

        variantDesc.reflectionInfo.vertexInputs.Add("POSITION");
        variantDesc.reflectionInfo.vertexInputs.Add("TEXCOORD0");

        fusionShader = new RPI::Shader();
        fusionShader->AddVariant(variantDesc);
    }

} // namespace CE

