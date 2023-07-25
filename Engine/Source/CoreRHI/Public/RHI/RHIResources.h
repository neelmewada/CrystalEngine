#pragma once

#include "RHIDefinitions.h"

namespace CE::RHI
{
    
    class CORERHI_API Resource
    {
    protected:
        Resource(ResourceType resourceType) : resourceType(resourceType)
        {}
    public:
        virtual ~Resource() = default;

        ResourceType GetResourceType() const 
        {
            return resourceType;
        }
        
    private:
        ResourceType resourceType = ResourceType::None;
    };

    class IDeviceObject
    {
    protected:
        IDeviceObject(DeviceObjectType type) : deviceObjectType(type)
        {}
    public:
        virtual ~IDeviceObject() {}

        virtual DeviceObjectType GetDeviceObjectType() const
        {
            return deviceObjectType;
        }
        
    private:
        DeviceObjectType deviceObjectType = DeviceObjectType::None;
    };

    class CORERHI_API Buffer : public Resource, public IDeviceObject
    {
    protected:
        Buffer() : Resource(ResourceType::Buffer), IDeviceObject(DeviceObjectType::Buffer)
        {}

    public:
        virtual ~Buffer() = default;
        
        virtual BufferBindFlags GetBindFlags() = 0;

        virtual void* GetHandle() = 0;

        virtual void UploadData(const BufferData& data) = 0;

    };
    
    class CORERHI_API Texture : public Resource, public IDeviceObject
    {
    protected:
        Texture() : Resource(ResourceType::Texture), IDeviceObject(DeviceObjectType::Texture)
        {}
        
    public:
        virtual ~Texture() = default;

        virtual void* GetHandle() = 0;

        virtual u32 GetWidth() = 0;
        virtual u32 GetHeight() = 0;
        virtual u32 GetDepth() = 0;
        virtual u32 GetBytesPerChannel() = 0;
        virtual u32 GetNumberOfChannels() = 0;

        virtual void UploadData(const void* pixels) = 0;
    };

    class CORERHI_API Sampler : public Resource
    {
    protected:
        Sampler() : Resource(ResourceType::Sampler)
        {}
        
    public:
        
        virtual ~Sampler() = default;
        
        virtual void* GetHandle() = 0;
        
    }

    /***********************************
    *   Shader
    */

    class CORERHI_API ShaderModule : public Resource
    {
    protected:
        ShaderModule() : Resource(ResourceType::ShaderModule)
        {}

    public:
        virtual ~ShaderModule() = default;

        // - Public API -

        virtual Name GetName() = 0;

        virtual bool IsValidShader() = 0;

        virtual ShaderStage GetShaderStage() = 0;

        bool IsVertexShader() { return GetShaderStage() == ShaderStage::Vertex; }
        bool IsFragmentShader() { return GetShaderStage() == ShaderStage::Fragment; }

    };

    /***********************************
    *   Pipeline
    */

    class CORERHI_API PipelineState : public Resource
    {
    protected:
        PipelineState(ResourceType type) : Resource(type)
        {}

    public:
        virtual ~PipelineState() = default;

        // - Public API -

        virtual bool IsGraphicsPipelineState() 
        {
            return GetResourceType() == ResourceType::GraphicsPipelineState; 
        }

        virtual bool IsComputePipelineState()
        {
            return GetResourceType() == ResourceType::ComputePipelineState;
        }


    };
    
} // namespace CE
