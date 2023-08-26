#pragma once

namespace CE
{
	ENUM(Flags)
	enum class ShaderStage
	{
		None = 0,
		Vertex = BIT(0),
		Fragment = BIT(1),

		Default = Vertex | Fragment,
	};

	ENUM()
	enum class ShaderBlobFormat
	{
		Spirv = 0
	};

	ENUM()
	enum class ShaderResourceType
	{
		None = 0,
		ConstantBuffer,
		StructuredBuffer,
		SamplerState,
		Texture2D,
		Texture3D,
		TextureCube,
		RWTexture2D,
		RWTexture3D,
		RWTextureCube,
	};

	STRUCT()
	struct CORESHADER_API SRGVariable
	{
		CE_STRUCT(SRGVariable)
	public:

		inline bool IsValid() const { return resourceType != ShaderResourceType::None; }

		inline u32 GetBinding() const { return binding; }

		inline const Name& GetName() const { return name; }
		inline const Name& GetActualName() const { return actualName; }

	protected:
		
		FIELD(ReadOnly)
		u32 binding = 0;

		FIELD(ReadOnly)
		Name name{};

		FIELD(ReadOnly)
		Name actualName{};

		FIELD(ReadOnly)
		ShaderResourceType resourceType = ShaderResourceType::None;

		FIELD(ReadOnly)
		int count = 1;

		friend class ShaderReflector;
	};

	STRUCT()
	struct CORESHADER_API SRGEntry
	{
		CE_STRUCT(SRGEntry)
	public:

		inline u32 GetFrequencyId() const { return frequencyId; }

		inline u32 GetNumVariables() const { return variables.GetSize(); }

		inline const SRGVariable& GetVariable(int index) const { return variables[index]; }


	protected:

		FIELD(ReadOnly)
		u32 frequencyId = 0; // The set number in vulkan, or register space in dx12.
		
		FIELD(ReadOnly)
		Array<SRGVariable> variables{};

		friend class ShaderReflector;
	};
    
	STRUCT()
	struct CORESHADER_API ShaderReflection
	{
		CE_STRUCT(ShaderReflection)
	public:

	protected:

		FIELD(ReadOnly)
		Array<SRGEntry> resourceGroups{};
		
		friend class ShaderReflector;
	};

} // namespace CE

#include "ShaderDefines.rtti.h"