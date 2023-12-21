#pragma once

namespace CE::RPI
{
	class PassTree;

	class CORERPI_API RenderPipeline final : public IntrusiveBase
	{
	public:

		virtual ~RenderPipeline();

	private:

		PassTree passTree{};

	};

	typedef Ptr<RenderPipeline> RenderPipelinePtr;
    
} // namespace CE::RPI
