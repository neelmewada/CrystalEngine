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

		virtual void CompileCrossQueueScopes(const FrameGraphCompilerRequest& compileRequest) = 0;

		virtual void CompileInternal(const FrameGraphCompilerRequest& compileRequest) = 0;

	};

} // namespace CE::RHI
