#pragma once

namespace CE::RHI
{
	ENUM()
	enum class SRGType
	{
		PerScene = 0,
		PerView,
		PerPass,
		PerSubPass,
		PerMaterial,
		PerObject,
		PerDraw,
        COUNT
	};
	ENUM_CLASS(SRGType);

	STRUCT()
	struct CORERHI_API SRGVariableDescriptor
	{
		CE_STRUCT(SRGVariableDescriptor)
	public:

		/// @brief Name of the variable.
		FIELD()
		Name name{};

		/// @brief Each variable has a unique binding slot, usually retrieved via reflection.
		FIELD()
		u32 bindingSlot = 0;

		FIELD()
		u32 arrayCount = 1;

		FIELD()
		ShaderResourceType type = ShaderResourceType::None;

		FIELD()
		b8 usesDynamicOffset = false;

		FIELD()
		RHI::Format format{};

		FIELD()
		ShaderStage shaderStages = ShaderStage::Default;

	};

	STRUCT()
	struct CORERHI_API ShaderResourceGroupLayout
	{
		CE_STRUCT(ShaderResourceGroupLayout)
	public:

		FIELD()
		SRGType srgType = SRGType::PerScene;

		FIELD()
		Array<SRGVariableDescriptor> variables{};

		void TryAdd(const SRGVariableDescriptor& variable)
		{
			for (int i = 0; i < variables.GetSize(); i++)
			{
				if (variables[i].name == variable.name)
				{
					variables[i].shaderStages |= variable.shaderStages;
					return;
				}
			}

			variables.Add(variable);
		}

		/// @brief Merge 'other' SRG layout into 'this' SRG layout.
		void Merge(const ShaderResourceGroupLayout& other)
		{
			if ((int)other.srgType > (int)srgType)
			{
				srgType = other.srgType;
			}

			for (const auto& otherVariable : other.variables)
			{
				bool exists = false;
				
				for (const auto& thisVariable : variables)
				{
					if (thisVariable.bindingSlot == otherVariable.bindingSlot || thisVariable.name == otherVariable.name)
					{
						exists = true;
						break;
					}
				}

				if (!exists)
				{
					variables.Add(otherVariable);
				}
			}
		}

	};
    
} // namespace CE::RHI

#include "ShaderResourceGroupLayout.rtti.h"