#pragma once

namespace CE::RHI
{
	ENUM()
	enum class ClearValueType
	{
		Float4,
		Int4,
		DepthStencil
	};

	struct ClearDepthStencil
	{
		f32 depth = 0;
		u8 stencil = 0;

		inline bool operator==(const ClearDepthStencil& rhs) const
		{
			return depth == rhs.depth && stencil == rhs.stencil;
		}
	};

	STRUCT()
	struct ClearValue
	{
		CE_STRUCT(ClearValue)
	public:

		ClearValue() = default;

		explicit ClearValue(const Vec4& float4);
		explicit ClearValue(const Vec4i& int4);
		explicit ClearValue(ClearDepthStencil depthStencil);

		bool operator==(const ClearValue& other) const;

		FIELD()
		ClearValueType clearType = ClearValueType::Float4;

		FIELD()
		Vec4 float4{};

		FIELD()
		Vec4i int4{};

		FIELD()
		ClearDepthStencil depthStencil{};
	};
    
} // namespace CE::RHI

#include "ClearValue.rtti.h"
