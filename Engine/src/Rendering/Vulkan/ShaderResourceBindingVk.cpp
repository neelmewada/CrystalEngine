
#include "ShaderResourceBindingVk.h"
#include <iostream>

using namespace Vox;

ShaderResourceVariableVk::ShaderResourceVariableVk(const ShaderResourceVariableVkCreateInfo& createInfo,
                                                   IShaderResourceBindingCallbacks* pReceiver)
{
    m_Set = createInfo.set;
    m_Binding = createInfo.binding;
    m_ResourceVariableType = createInfo.resourceType;
    m_DescriptorCount = createInfo.descriptorCount;
    m_Name = createInfo.name;
    m_pReceiver = pReceiver;
    m_BindingRef = createInfo.bindingRef;
}

ShaderResourceVariableVk::~ShaderResourceVariableVk()
{

}

void ShaderResourceVariableVk::Set(IDeviceObject* pObject)
{
    m_pReceiver->BindShaderResource(m_BindingRef, pObject, m_Set, m_Binding,
                                    m_DescriptorCount, m_ResourceVariableType);
}

#pragma region ShaderResourceBindingVk

ShaderResourceBindingVk::ShaderResourceBindingVk(const ShaderResourceBindingVkCreateInfo& createInfo,
                                                 IShaderResourceBindingCallbacks* pReceiver)
{
    m_VariableDefinitions = createInfo.variableDefinitions;
    m_pReceiver = pReceiver;

    for (int i = 0; i < m_VariableDefinitions.size(); ++i)
    {
        auto varDef = m_VariableDefinitions[i];
        if (varDef.name.empty()) continue;

        ShaderResourceVariableVkCreateInfo varInfo = {};
        varInfo.name = varDef.name;
        varInfo.descriptorCount = !varDef.isArray ? 1 : varDef.members.size();
        varInfo.set = varDef.set;
        varInfo.binding = varDef.binding;
        varInfo.resourceType = varDef.type;
        varInfo.bindingRef = this;

        auto* variableBinding = new ShaderResourceVariableVk(varInfo, m_pReceiver);
        m_VariableBindings.push_back(variableBinding);
    }
}

ShaderResourceBindingVk::~ShaderResourceBindingVk()
{
    for (int i = 0; i < m_VariableBindings.size(); ++i)
    {
        delete m_VariableBindings[i];
    }
    m_VariableBindings.clear();
}

IShaderResourceVariable* ShaderResourceBindingVk::GetVariableByName(const char* pName)
{
    auto iter = std::find_if(m_VariableBindings.begin(), m_VariableBindings.end(),
        [pName](const ShaderResourceVariableVk* item) {
            return item->m_Name == pName;
        });

    if (iter == m_VariableBindings.end())
        return nullptr;
    return *iter;
}

#pragma endregion
