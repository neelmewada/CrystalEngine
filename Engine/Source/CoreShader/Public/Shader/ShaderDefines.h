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
	struct CORESHADER_API ShaderReflection
	{
		CE_STRUCT(ShaderReflection)
	public:


	};

} // namespace CE

#include "ShaderDefines.rtti.h"