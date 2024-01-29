#pragma once

namespace CE::RPI
{

	class CORERPI_API Material final
	{
	public:

		Material();
		~Material();

	private:

		RHI::PipelineState* pipelineState = nullptr;
	};
    
} // namespace CE::RPI
