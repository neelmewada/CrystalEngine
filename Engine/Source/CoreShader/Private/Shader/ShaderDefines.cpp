#include "CoreShader.h"

namespace CE
{

    RHI::ShaderResourceGroupLayout& ShaderReflection::FindOrAdd(RHI::SRGType srgType)
    {
		for (auto& entry : srgLayouts)
		{
			if (entry.srgType == srgType)
				return entry;
		}

		RHI::ShaderResourceGroupLayout entry{};
		entry.srgType = srgType;
		srgLayouts.Add(entry);

		return srgLayouts.Top();
    }

	const VariableBindingMap& ShaderReflection::GetVariableNameMap() const
	{
		if (variableNameToBindingSlot.IsEmpty())
		{
			for (const auto& srgEntry : srgLayouts)
			{
				for (const auto& variable : srgEntry.variables)
				{
					variableNameToBindingSlot[variable.name] = variable.bindingSlot;
				}
			}
		}

		return variableNameToBindingSlot;
	}

	void ShaderReflection::OnAfterDeserialize()
	{
		variableNameToBindingSlot.Clear();

		GetVariableNameMap();
	}

	void SRGEntry::TryAdd(const SRGVariable& variable, ShaderStage stage)
	{
		for (auto& var : variables)
		{
			if (var.GetName() == variable.GetName() && var.GetBindingSlot() == variable.GetBindingSlot())
			{
				var.shaderStages |= stage;
				return;
			}
		}
		
		variables.Add(variable);
		variables.Top().shaderStages |= stage;
	}

} // namespace CE
