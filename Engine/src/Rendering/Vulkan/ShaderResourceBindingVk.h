#pragma once

#include "Rendering/IShaderResourceBinding.h"
#include "ShaderVk.h"
#include "RenderContextVk.h"
#include "TypesVk.h"

#include <vulkan/vulkan.h>

namespace Vox
{

class RenderContextVk;
struct ResourceLocation;

class IShaderResourceBindingCallbacks
{
protected:
    virtual ~IShaderResourceBindingCallbacks() {}

public: // Public API
    virtual void BindDeviceObject(IDeviceObject* pDeviceObject, Uint32 set, Uint32 binding,
                                  Uint32 descriptorCount, ShaderResourceVariableType resourceType) = 0;

    virtual void OnDynamicOffsetUpdated(Uint32 set, Uint32 binding, Uint32 dynamicOffsetValue) = 0;
};

class ShaderResourceBindingVk;


struct ShaderResourceVariableVkCreateInfo
{
    std::string name;
    Uint32 set;
    Uint32 binding;
    Uint32 descriptorCount;
    ShaderResourceVariableType resourceType;
    bool usesDynamicOffset;
};

class ShaderResourceVariableVk : public IShaderResourceVariable
{
public:
    friend class ShaderResourceBindingVk;
    ShaderResourceVariableVk(const ShaderResourceVariableVkCreateInfo& createInfo, IShaderResourceBindingCallbacks* pReceiver);
    ~ShaderResourceVariableVk();

    DELETE_COPY_CONSTRUCTORS(ShaderResourceVariableVk);

public: // Public API
    void Set(IDeviceObject* pObject) override;
    void Set(IDeviceObject* pObject, Uint32 dynamicOffset) override;
    void SetDynamicOffset(Uint32 offset) override;
    Uint32 GetDynamicOffset() override;

private: // Internal Members
    std::string m_Name;
    Uint32 m_Set = 0;
    Uint32 m_Binding = 0;
    Uint32 m_DescriptorCount = 0;
    ShaderResourceVariableType m_ResourceVariableType;
    IShaderResourceBindingCallbacks* m_pReceiver;
    bool m_UsesDynamicOffset;

    // - Data
    Uint32 m_DynamicOffset = 0;
    IDeviceObject* m_pBoundObject = nullptr;
};

struct ShaderResourceBindingVkCreateInfo
{
    Uint32 firstSet;
    Uint32 setCount;
    int maxSimultaneousFrames;
    ResourceBindingFrequency bindingFrequency;
};

struct ShaderVariableMetaData
{
    Uint32 set;
    Uint32 binding;
    std::string name;
    bool isArray;
    Uint32 arraySize;
    ShaderResourceVariableType resourceVarType;
    bool immutableSamplerExists;
    ShaderStageFlags stages;
    ShaderResourceVariableFlags flags;
};


class ShaderResourceBindingVk : public IShaderResourceBinding, public IShaderResourceBindingCallbacks
{
public:
    ShaderResourceBindingVk(const ShaderResourceBindingVkCreateInfo& createInfo,
                            const std::vector<ShaderVariableMetaData>& variableMetaData,
                            const std::vector<VkDescriptorSetLayout>& setLayouts,
                            DeviceContextVk* pDevice, RenderContextVk* pRenderCtx);
    ~ShaderResourceBindingVk();

    DELETE_COPY_CONSTRUCTORS(ShaderResourceBindingVk)

public: // Public API
    IShaderResourceVariable* GetVariableByName(const char* pName) override;

    void BindDeviceObject(IDeviceObject *pDeviceObject,
                          Uint32 set, Uint32 binding, Uint32 descriptorCount,
                          ShaderResourceVariableType resourceType) override;

    void OnDynamicOffsetUpdated(Uint32 set, Uint32 binding, Uint32 dynamicOffsetValue) override;

    void CmdBindDescriptorSets(VkCommandBuffer cmdBuffer, int currentFrame);

private: // Internal API


private: // Internal Members
    DeviceContextVk* m_pDevice = nullptr;
    RenderContextVk* m_pRenderContext = nullptr;

    Uint32 m_FirstSet;
    Uint32 m_SetCount;
    int m_MaxSimultaneousFrames;
    ResourceBindingFrequency m_BindingFrequency;

    Uint32 m_DynamicDescriptorCount;
    std::vector<Uint32> m_DynamicOffsets;
    std::map<ResourceLocation, Uint32> m_VariablePositionInDynamicOffsetsList;

    std::vector<VkDescriptorSet> m_DescriptorSets;
    std::vector<ShaderResourceVariableVk*> m_VariableBindings;
    std::map<ResourceLocation, ShaderVariableMetaData> m_VariableMetaData;

private: // Vulkan Members
    VkDescriptorPool m_DescriptorPool = nullptr;

};

}

