#pragma once

#include "Rendering/IShaderResourceBinding.h"
#include "ShaderVk.h"
#include "TypesVk.h"

#include <vulkan/vulkan.h>

namespace Vox
{

class IShaderResourceBindingCallbacks
{
protected:
    virtual ~IShaderResourceBindingCallbacks() {}

public: // Public API
    virtual void BindShaderResource(IDeviceObject* pDeviceObject, Uint32 set, Uint32 binding,
                                    Uint32 descriptorCount, ShaderResourceVariableType resourceType) = 0;

};

struct ShaderResourceVariableVkCreateInfo
{
    std::string name;
    Uint32 set;
    Uint32 binding;
    Uint32 descriptorCount;
    ShaderResourceVariableType resourceType;
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

private: // Internal Members
    std::string m_Name;
    Uint32 m_Set = 0;
    Uint32 m_Binding = 0;
    Uint32 m_DescriptorCount = 0;
    ShaderResourceVariableType m_ResourceVariableType;
    IShaderResourceBindingCallbacks* m_pReceiver;
};

struct ShaderResourceBindingVkCreateInfo
{
    const std::vector<ShaderResourceVariableDefinition>& variableDefinitions;
};

class ShaderResourceBindingVk : public IShaderResourceBinding
{
public:
    ShaderResourceBindingVk(const ShaderResourceBindingVkCreateInfo& createInfo, IShaderResourceBindingCallbacks* pReceiver);
    ~ShaderResourceBindingVk();

    DELETE_COPY_CONSTRUCTORS(ShaderResourceBindingVk)

public: // Public API
    IShaderResourceVariable* GetVariableByName(const char* pName) override;

private: // Internal API


private: // Internal Members
    IShaderResourceBindingCallbacks* m_pReceiver;
    std::vector<ShaderResourceVariableDefinition> m_VariableDefinitions;
    std::vector<ShaderResourceVariableVk*> m_VariableBindings;

private: // Vulkan Members

};

}

