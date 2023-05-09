#pragma once

#include "RHIDefinitions.h"

namespace CE
{
    
    class CORERHI_API RHIResource
    {
    protected:
        RHIResource(RHIResourceType resourceType) : resourceType(resourceType)
        {}
    public:
        virtual ~RHIResource() = default;

        RHIResourceType GetResourceType() const 
        {
            return resourceType;
        }
        
    private:
        RHIResourceType resourceType = RHIResourceType::None;
    };

    class IRHIDeviceObject
    {
    protected:
        IRHIDeviceObject(RHIDeviceObjectType type) : deviceObjectType(type)
        {}
    public:
        virtual ~IRHIDeviceObject() {}

        virtual RHIDeviceObjectType GetDeviceObjectType() const
        {
            return deviceObjectType;
        }
        
    private:
        RHIDeviceObjectType deviceObjectType = RHIDeviceObjectType::None;
    };

    class CORERHI_API RHIBuffer : public RHIResource, public IRHIDeviceObject
    {
    protected:
        RHIBuffer() : RHIResource(RHIResourceType::Buffer), IRHIDeviceObject(RHIDeviceObjectType::Buffer)
        {}

    public:
        virtual ~RHIBuffer() = default;
        
        virtual RHIBufferBindFlags GetBindFlags() = 0;

        virtual void* GetHandle() = 0;

        virtual void UploadData(const RHIBufferData& data) = 0;

    };
    
    class CORERHI_API RHITexture : public RHIResource, public IRHIDeviceObject
    {
    protected:
        RHITexture() : RHIResource(RHIResourceType::Texture), IRHIDeviceObject(RHIDeviceObjectType::Texture)
        {}
        
    public:
        virtual ~RHITexture() = default;

        virtual void* GetHandle() = 0;

        virtual u32 GetWidth() = 0;
        virtual u32 GetHeight() = 0;
        virtual u32 GetDepth() = 0;
        virtual u32 GetBytesPerChannel() = 0;
        virtual u32 GetNumberOfChannels() = 0;

        virtual void UploadData(const void* pixels) = 0;
    };

    /***********************************
    *   Shader
    */

    class CORERHI_API RHIShaderModule : public RHIResource
    {
    protected:
        RHIShaderModule() : RHIResource(RHIResourceType::ShaderModule)
        {}

    public:
        virtual ~RHIShaderModule() = default;

        // - Public API -

        virtual Name GetName() = 0;

        virtual bool IsValidShader() = 0;

        virtual RHIShaderStage GetShaderStage() = 0;

        bool IsVertexShader() { return GetShaderStage() == RHIShaderStage::Vertex; }
        bool IsFragmentShader() { return GetShaderStage() == RHIShaderStage::Fragment; }

    };

    /***********************************
    *   Pipeline
    */

    class CORERHI_API RHIPipelineState : public RHIResource
    {
    protected:
        RHIPipelineState(RHIResourceType type) : RHIResource(type)
        {}

    public:
        virtual ~RHIPipelineState() = default;

        // - Public API -

        virtual bool IsGraphicsPipelineState() 
        {
            return GetResourceType() == RHIResourceType::GraphicsPipelineState; 
        }

        virtual bool IsComputePipelineState()
        {
            return GetResourceType() == RHIResourceType::ComputePipelineState;
        }


    };
    
} // namespace CE
