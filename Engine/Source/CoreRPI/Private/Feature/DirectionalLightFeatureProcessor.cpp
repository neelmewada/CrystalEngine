#include "CoreRPI.h"

namespace CE::RPI
{
    void DirectionalLightInstance::Init(DirectionalLightFeatureProcessor* fp)
    {
        if (flags.initialized)
            return;

        flags.initialized = true;

        RHI::DrawListTag shadowTag = RPISystem::Get().GetBuiltinDrawListTag(BuiltinDrawItemTag::Shadow);
        RPI::Shader* shader = RPISystem::Get().GetStandardShader()->GetShader(shadowTag);
        const RHI::ShaderResourceGroupLayout& perViewLayout = shader->GetDefaultVariant()->GetSrgLayout(RHI::SRGType::PerView);

        viewSrg = RHI::gDynamicRHI->CreateShaderResourceGroup(perViewLayout);

        RHI::BufferDescriptor viewConstantBufferDesc;
        viewConstantBufferDesc.name = "DirectionalLight ViewConstants";
        viewConstantBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
        viewConstantBufferDesc.bufferSize = sizeof(PerViewConstants);
        viewConstantBufferDesc.defaultHeapType = RHI::MemoryHeapType::Upload;
        viewConstantBufferDesc.structureByteStride = viewConstantBufferDesc.bufferSize;

        for (int i = 0; i < viewConstantBuffers.GetSize(); ++i)
        {
            viewConstantBuffers[i] = RHI::gDynamicRHI->CreateBuffer(viewConstantBufferDesc);
            viewSrg->Bind(i, "_PerViewData", viewConstantBuffers[i]);
        }

        viewSrg->FlushBindings();
    }

    void DirectionalLightInstance::Deinit(DirectionalLightFeatureProcessor* fp)
    {
        if (!flags.initialized)
            return;

        delete viewSrg; viewSrg = nullptr;

        for (int i = 0; i < viewConstantBuffers.GetSize(); ++i)
        {
            delete viewConstantBuffers[i]; viewConstantBuffers[i] = nullptr;
        }

        flags.initialized = false;
    }

    void DirectionalLightInstance::UpdateSrgs(u32 imageIndex)
    {
        void* data;
        viewConstantBuffers[imageIndex]->Map(0, viewConstantBuffers[imageIndex]->GetBufferSize(), &data);
        {
            auto constants = (PerViewConstants*)data;
            constants->pixelResolution = Vec2(512, 512);
            constants->viewMatrix = viewMatrix;
            constants->projectionMatrix = projectionMatrix;
            constants->viewProjectionMatrix = viewProjectionMatrix;
            constants->viewPosition = viewPosition;
        }
        viewConstantBuffers[imageIndex]->Unmap();
    }

    DirectionalLightFeatureProcessor::DirectionalLightFeatureProcessor()
    {
	    
    }

    DirectionalLightFeatureProcessor::~DirectionalLightFeatureProcessor()
    {
        auto parallelRanges = lightInstances.GetParallelRanges();

        for (const auto& range : parallelRanges)
        {
            for (auto it = range.begin; it != range.end; ++it)
            {
                it->Deinit(this);
            }
        }

        for (int i = 0; i < directionalLightBuffers.GetSize(); ++i)
        {
            delete directionalLightBuffers[i]; directionalLightBuffers[i] = nullptr;
        }
    }

    DirectionalLightHandle DirectionalLightFeatureProcessor::AcquireLight(const DirectionalLightHandleDescriptor& desc)
    {
        DirectionalLightHandle handle = lightInstances.Insert({});
        handle->scene = GetScene();
        

        return handle;
    }

    bool DirectionalLightFeatureProcessor::ReleaseLight(DirectionalLightHandle& handle)
    {
        if (handle.IsValid())
        {
            handle->Deinit(this);
            lightInstances.Remove(handle);
            return true;
        }

        return false;
    }

    void DirectionalLightFeatureProcessor::Simulate(const SimulatePacket& packet)
    {
	    Super::Simulate(packet);

        if (directionalLightBuffers[0] == nullptr)
        {
            auto sceneSrg = scene->GetShaderResourceGroup();

            const RHI::SRGVariableDescriptor& directionalLightsVariable = sceneSrg->GetLayout().FindVariable("_DirectionalLightsArray");
            maxDirectionalLights = directionalLightsVariable.structMembers[0].arrayCount;

            RHI::BufferDescriptor directionalLightBufferDesc;
            directionalLightBufferDesc.name = "Directional Lights Array";
            directionalLightBufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
            directionalLightBufferDesc.structureByteStride = sizeof(DirectionalLightConstants);
            directionalLightBufferDesc.bufferSize = maxDirectionalLights * directionalLightBufferDesc.structureByteStride;
            directionalLightBufferDesc.defaultHeapType = MemoryHeapType::Upload;

            for (int i = 0; i < directionalLightBuffers.GetSize(); ++i)
            {
	            directionalLightBuffers[i] = RHI::gDynamicRHI->CreateBuffer(directionalLightBufferDesc);
	            sceneSrg->Bind(i, "_DirectionalLightsArray", directionalLightBuffers[i]);
            }

            sceneSrg->FlushBindings();
        }

        scene->GetLightConstants().totalDirectionalLights = Math::Min((int)lightInstances.GetCount(), maxDirectionalLights);
    }

    void DirectionalLightFeatureProcessor::Render(const RenderPacket& packet)
    {
	    Super::Render(packet);

        u32 imageIndex = packet.imageIndex;

        void* data;
        directionalLightBuffers[imageIndex]->Map(0, directionalLightBuffers[imageIndex]->GetBufferSize(), &data);
        {
            DirectionalLightConstants* constantsArray = (DirectionalLightConstants*)data;

            int lightCount = Math::Min((int)lightInstances.GetCount(), maxDirectionalLights);
            int i = 0;

            for (auto& lightInstance : lightInstances)
            {
                if (i >= lightCount)
                    break;

                if (!lightInstance.flags.initialized)
                {
                    lightInstance.Init(this);
                }

                lightInstance.UpdateSrgs(imageIndex);

                DirectionalLightConstants* constants = constantsArray + i;
                constants->direction = lightInstance.direction;
                constants->colorAndIntensity = lightInstance.colorAndIntensity;
                constants->lightSpaceMatrix = lightInstance.viewProjectionMatrix;
                constants->temperature = lightInstance.temperature;

                i++;
            }
        }
        directionalLightBuffers[imageIndex]->Unmap();
    }

    int DirectionalLightFeatureProcessor::GetMaxDirectionalLights()
    {
        if (maxDirectionalLights == 0 && scene != nullptr)
        {
            RHI::ShaderResourceGroup* srg = scene->GetShaderResourceGroup();
            if (srg != nullptr)
            {
                const RHI::SRGVariableDescriptor& directionalLightsVariable = srg->GetLayout().FindVariable("_DirectionalLightsArray");
                maxDirectionalLights = (int)directionalLightsVariable.structMembers[0].arrayCount;
            }
        }

        return maxDirectionalLights;
    }

} // namespace CE::RPI
