#pragma once

namespace CE
{

	ENUM()
	enum class ShaderBlobFormat
	{
		Spirv = 0
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
