#pragma once

namespace CE::RHI
{
	class FrameScheduler;

	struct FrameGraphCompileRequest
	{
		FrameGraph* frameGraph = nullptr;

		TransientMemoryPool* transientPool = nullptr;

		u32 numFramesInFlight = 1;

		bool shrinkPool = false;
	};
    
	class CORERHI_API FrameGraphCompiler
	{
	protected:
		FrameGraphCompiler() = default;

	public:

		virtual ~FrameGraphCompiler() = default;

		void Compile(const FrameGraphCompileRequest& compileRequest);

	protected:

		void CompileScopes(const FrameGraphCompileRequest& compileRequest);

		void CompileTransientAttachments(const FrameGraphCompileRequest& compileRequest);

		virtual void CompileScopesInternal(const FrameGraphCompileRequest& compileRequest) = 0;

		virtual void CompileInternal(const FrameGraphCompileRequest& compileRequest) = 0;

	};

} // namespace CE::RHI
