#include "CoreShader.h"

namespace CE
{

    SRGEntry& ShaderReflection::FindOrAdd(u32 frequencyId)
    {
		for (auto& entry : resourceGroups)
		{
			if (entry.GetFrequencyId() == frequencyId)
				return entry;
		}

		SRGEntry entry{};
		entry.frequencyId = frequencyId;
		resourceGroups.Add(entry);

		return resourceGroups.Top();
    }

	const VariableBindingMap& ShaderReflection::GetVariableNameMap() const
	{
		if (variableNameToBindingSlot.IsEmpty())
		{
			for (const SRGEntry& srgEntry : resourceGroups)
			{
				for (const SRGVariable& variable : srgEntry.variables)
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

		for (const SRGEntry& srgEntry : resourceGroups)
		{
			for (const SRGVariable& variable : srgEntry.variables)
			{
				variableNameToBindingSlot[variable.name] = variable.bindingSlot;
			}
		}
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
