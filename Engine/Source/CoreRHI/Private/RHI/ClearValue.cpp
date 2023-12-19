#include "CoreRHI.h"

namespace CE::RHI
{
    ClearValue::ClearValue(const Vec4& float4) : clearType(RHI::ClearValueType::Float4)
    {
		this->float4 = float4;
    }

    ClearValue::ClearValue(const Vec4i& int4) : clearType(RHI::ClearValueType::Int4)
    {
		this->int4 = int4;
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
			return float4 == other.float4;
		case ClearValueType::Int4:
			return int4 == other.int4;
		case ClearValueType::DepthStencil:
			return depthStencil == other.depthStencil;
		default:
			return false;
		}
	}

} // namespace CE::RHI
