#pragma once

namespace CE::RHI
{
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

	struct ClearValue
	{
		ClearValue() = default;

		explicit ClearValue(const Vec4& float4);
		explicit ClearValue(u32 uint4[4]);
		explicit ClearValue(ClearDepthStencil depthStencil);

		bool operator==(const ClearValue& other) const;

		ClearValueType clearType = ClearValueType::Float4;

		union {
			f32 float4[4] = {};
			u32 uint4[4];
			ClearDepthStencil depthStencil;
		};
	};
    
} // namespace CE::RHI

