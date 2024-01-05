#pragma once

namespace CE::RHI
{

	class CORERHI_API FrameGraphBuilder final
	{
	public:
		FrameGraphBuilder() = default;

		void Begin(FrameGraph* frameGraph);

		void BeginScope();

		bool EndScope();

		bool End();

	private:

		FrameGraph* frameGraph = nullptr;
		Scope* currentScope = nullptr;

	};
    
} // namespace CE::RHI
