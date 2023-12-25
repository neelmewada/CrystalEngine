#pragma once

namespace CE::RPI
{
	/// @brief A pass that does some GPU work.
	CLASS(Abstract)
	class CORERPI_API GpuPass : public Pass
	{
		CE_CLASS(GpuPass, Pass)
	public:



	private:

		RPI_PASS(GpuPass)
	};
    
} // namespace CE::RPI

#include "GpuPass.rtti.h"
