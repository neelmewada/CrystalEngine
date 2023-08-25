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

} // namespace CE

#include "ShaderDefines.rtti.h"
