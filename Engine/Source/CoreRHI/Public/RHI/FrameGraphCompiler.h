#pragma once

namespace CE::RHI
{
	struct FrameGraphCompileRequest
	{
		FrameGraph* frameGraph = nullptr;

		TransientMemoryPool* transientPool = nullptr;

		u32 numFramesInFlight = 1;
	};
    
	class CORERHI_API FrameGraphCompiler
	{
	protected:
		FrameGraphCompiler() = default;

	public:

		virtual ~FrameGraphCompiler() = default;

		void Compile(const FrameGraphCompileRequest& compileRequest);

	protected:

		void CompileTransientAttachments(const FrameGraphCompileRequest& compileRequest);

		virtual void CompileCrossQueueScopes(const FrameGraphCompileRequest& compileRequest) = 0;

		virtual void CompileInternal(const FrameGraphCompileRequest& compileRequest) = 0;

	};

} // namespace CE::RHI
