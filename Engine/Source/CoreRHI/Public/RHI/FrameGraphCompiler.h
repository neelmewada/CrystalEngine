#pragma once

namespace CE::RHI
{
	struct FrameGraphCompilerRequest
	{
		FrameGraph* frameGraph = nullptr;

		TransientMemoryPool* transientPool = nullptr;
	};
    
	class CORERHI_API FrameGraphCompiler
	{
	protected:
		FrameGraphCompiler() = default;

	public:

		virtual ~FrameGraphCompiler() = default;

		void Compile(const FrameGraphCompilerRequest& compileRequest);

	protected:


	};

} // namespace CE::RHI
