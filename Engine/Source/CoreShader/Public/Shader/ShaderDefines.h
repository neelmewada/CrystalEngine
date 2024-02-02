#pragma once

namespace CE
{

	ENUM()
	enum class ShaderBlobFormat
	{
		Spirv = 0
	};

	STRUCT()
	struct CORESHADER_API SRGVariable
	{
		CE_STRUCT(SRGVariable)
	public:

		inline bool IsValid() const { return resourceType != RHI::ShaderResourceType::None; }

		inline u32 GetBindingSlot() const { return bindingSlot; }

		inline const Name& GetName() const { return name; }
		inline const Name& GetInternalName() const { return internalName; }
		
		FIELD(ReadOnly)
		u32 bindingSlot = 0;

		FIELD(ReadOnly)
		Name name{};

		FIELD(ReadOnly)
		Name internalName{};

		FIELD(ReadOnly)
		RHI::ShaderResourceType resourceType = RHI::ShaderResourceType::None;

		FIELD(ReadOnly)
		ShaderStage shaderStages = ShaderStage::All;

		FIELD(ReadOnly)
		Array<ShaderStructMember> structMembers{};

		FIELD(ReadOnly)
		int count = 1;

		friend class ShaderReflector;
		friend class Shader;
		friend struct SRGEntry;
	};

	STRUCT()
	struct CORESHADER_API SRGEntry
	{
		CE_STRUCT(SRGEntry)
	public:

		inline u32 GetFrequencyId() const { return frequencyId; }

		inline u32 GetNumVariables() const { return variables.GetSize(); }

		inline const SRGVariable& GetVariable(int index) const { return variables[index]; }


		FIELD(ReadOnly)
		u32 frequencyId = 0; // The set number in vulkan, or register space in dx12.
		
		FIELD(ReadOnly)
		Array<SRGVariable> variables{};

		void TryAdd(const SRGVariable& variable, ShaderStage stage);

		friend class ShaderReflector;
		friend struct ShaderReflection;
		friend class Shader;
	};

	typedef HashMap<Name, int> VariableBindingMap;
    
	STRUCT()
	struct CORESHADER_API ShaderReflection
	{
		CE_STRUCT(ShaderReflection)
	public:

		inline bool IsValid() const
		{
			return srgLayouts.NonEmpty();
		}

		RHI::ShaderResourceGroupLayout& FindOrAdd(RHI::SRGType srgType);

		FIELD(ReadOnly)
		Array<RHI::ShaderResourceGroupLayout> srgLayouts{};

		FIELD(ReadOnly)
		Array<ShaderSemantic> vertexInputs{};

		const VariableBindingMap& GetVariableNameMap() const;

	private:

		void OnAfterDeserialize();

		mutable VariableBindingMap variableNameToBindingSlot{};
		
		friend class ShaderReflector;
		friend class Shader;
	};

} // namespace CE

#include "ShaderDefines.rtti.h"
