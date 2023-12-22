#pragma once

namespace CE::RPI
{
	class PassTree;

	struct PipelineViews
	{
		PipelineViewTag viewTag{};

		Array<ViewPtr> views{};
	};

	using PipelineViewsByTag = HashMap<PipelineViewTag, PipelineViews>;

	/// @brief Descriptor used to create a Render Pipeline.
	struct RenderPipelineDesc
	{
		/// @brief Name of the render pipeline.
		Name name{};

		/// @brief Name of the pass template to instantiate as root pass
		Name rootPassTemplate{};

		/// @brief Name tag of the main view.
		Name mainViewTag = "MainCamera";

		/// @brief Name tags of each draw lists
		HashMap<u8, Name> drawListTagNames{};
	};

	class CORERPI_API RenderPipeline final
	{
	public:

		virtual ~RenderPipeline();

		static RenderPipeline* CreateBuiltin(RHI::RenderTarget* renderTarget);

		static RenderPipeline* Create(RHI::RenderTarget* renderTarget, const RenderPipelineDesc& desc);
		
	private:

		void Initialize(const RenderPipelineDesc& desc);

	protected:

		RenderPipeline() = default;

		/// @brief The output render target of this render pipeline.
		RHI::RenderTarget* renderTarget = nullptr;

		/// @brief Name of the pipeline
		Name name{};

		RenderPipelineDesc desc{};

		/// @brief Name of the pass template to instantiate as root pass
		Name rootPassTemplateName{};

		/// @brief Name tag of the main view.
		Name mainViewTag = "MainCamera";

		/// @brief Scene this render pipeline belongs to.
		Scene* scene = nullptr;

		/// @brief The pass tree hierarchy. Pipeline owns & manages the passes.
		PassTree passTree{};

		/// @brief A hash map of all views owned by this pipeline accessed by their respective tags
		PipelineViewsByTag pipelineViewsByTag{};

	};
    
} // namespace CE::RPI
