#pragma once

namespace CE
{
	ENUM(Flags)
	enum class VertexInputAttribute
	{
		None = 0,
		Position = BIT(0),
		Normal = BIT(1),
		Tangent = BIT(2),
		Color = BIT(3),
		UV0 = BIT(4),
		UV1 = BIT(5),
		UV2 = BIT(6),
		UV3 = BIT(7),
	};
	ENUM_CLASS_FLAGS(VertexInputAttribute);

	CORESHADER_API SIZE_T GetVertexInputTypeSize(VertexInputAttribute input);

	ENUM(Flags)
	enum class ShaderStage
	{
		None = 0,
		Vertex = BIT(0),
		Fragment = BIT(1),

		Default = Vertex | Fragment,
	};
	ENUM_CLASS_FLAGS(ShaderStage);

	ENUM()
	enum class ShaderBlobFormat
	{
		Spirv = 0
	};

	ENUM()
	enum class ShaderResourceType
	{
		None = 0,
		/// Uniform buffer in vulkan terms
		ConstantBuffer,
		/// Storage Buffer in vulkan terms
		StructuredBuffer,
		SamplerState,
		Texture1D,
		Texture2D,
		Texture3D,
		TextureCube,
		// image2D in vulkan
		RWTexture2D,
	};

	STRUCT()
	struct CORESHADER_API SRGVariable
	{
		CE_STRUCT(SRGVariable)
	public:

		inline bool IsValid() const { return resourceType != ShaderResourceType::None; }

		inline u32 GetBinding() const { return binding; }

		inline const Name& GetName() const { return name; }
		inline const Name& GetInternalName() const { return internalName; }

	protected:
		
		FIELD(ReadOnly)
		u32 binding = 0;

		FIELD(ReadOnly)
		Name name{};

		FIELD(ReadOnly)
		Name internalName{};

		FIELD(ReadOnly)
		ShaderResourceType resourceType = ShaderResourceType::None;

		FIELD(ReadOnly)
		int count = 1;

		friend class ShaderReflector;
		friend class Shader;
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
		friend struct ShaderReflection;
		friend class Shader;
	};
    
	STRUCT()
	struct CORESHADER_API ShaderReflection
	{
		CE_STRUCT(ShaderReflection)
	public:

		inline bool IsValid() const
		{
			return resourceGroups.NonEmpty();
		}

	protected:

		SRGEntry& FindOrAdd(u32 frequencyId);

		FIELD(ReadOnly)
		Array<SRGEntry> resourceGroups{};
		
		friend class ShaderReflector;
		friend class Shader;
	};

} // namespace CE

#include "ShaderDefines.rtti.h"