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

    ENUM()
    enum class ShaderStructMemberType
    {
        None = 0,
        UInt, Int,
        Float,
        Float2,
        Float3,
        Float4,
        Float4x4,
		Struct
    };
    ENUM_CLASS_FLAGS(ShaderStructMemberType);

    STRUCT()
    struct CORERHI_API ShaderStructMember
    {
        CE_STRUCT(ShaderStructMember)
    public:

        FIELD()
        Name name{};

        FIELD()
        ShaderStructMemberType dataType{};

		FIELD()
		u32 arrayCount = 1; // Static arrays
        
		FIELD()
		Array<ShaderStructMember> nestedMembers{};
    };

    ENUM()
    enum class SRGVariableFlags
    {
        None = 0,
        RarelyUpdated = BIT(0)
    };
    ENUM_CLASS_FLAGS(SRGVariableFlags);

	STRUCT()
	struct CORERHI_API SRGVariableDescriptor
	{
		CE_STRUCT(SRGVariableDescriptor)
	public:

		SRGVariableDescriptor() {}

		SRGVariableDescriptor(const Name& name, u32 bindingSlot, ShaderResourceType type, ShaderStage shaderStages)
			: name(name), bindingSlot(bindingSlot), type(type), shaderStages(shaderStages)
		{}

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
        SRGVariableFlags flags = SRGVariableFlags::None;
        
		FIELD()
		RHI::Format format{};

		FIELD()
		ShaderStage shaderStages = ShaderStage::Default;
        
        FIELD()
        Array<ShaderStructMember> structMembers{};
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

		Self& TryAdd(const SRGVariableDescriptor& variable)
		{
			for (int i = 0; i < variables.GetSize(); i++)
			{
				if (variables[i].name == variable.name)
				{
					variables[i].shaderStages |= variable.shaderStages;
					return *this;
				}
			}

			variables.Add(variable);

			return *this;
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
