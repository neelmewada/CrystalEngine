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
	struct CORERHI_API SRGVariableDesc
	{
		CE_STRUCT(SRGVariableDesc)
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
		Array<SRGVariableDesc> variables{};

	};
    
} // namespace CE::RHI

#include "ShaderResourceGroupLayout.rtti.h"