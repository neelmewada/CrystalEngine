#include "CoreRHI.h"

namespace CE::RHI
{
    ClearValue::ClearValue(const Vec4& float4) : clearType(RHI::ClearValueType::Float4)
    {
		memcpy(this->float4, float4.xyzw, sizeof(f32) * 4);
    }

    ClearValue::ClearValue(u32 uint4[4]) : clearType(RHI::ClearValueType::Int4)
    {
		memcpy(this->uint4, uint4, sizeof(u32) * 4);
    }

    ClearValue::ClearValue(ClearDepthStencil depthStencil) : clearType(RHI::ClearValueType::DepthStencil)
    {
		this->depthStencil = depthStencil;
    }

	bool ClearValue::operator==(const ClearValue& other) const
	{
		if (clearType != other.clearType)
			return false;
		
		switch (clearType)
		{
		case ClearValueType::Float4:
			return memcmp(float4, other.float4, sizeof(f32) * 4) == 0;
		case ClearValueType::Int4:
			return memcmp(uint4, other.uint4, sizeof(u32) * 4) == 0;
		case ClearValueType::DepthStencil:
			return depthStencil == other.depthStencil;
		default:
			return false;
		}
	}

} // namespace CE::RHI
