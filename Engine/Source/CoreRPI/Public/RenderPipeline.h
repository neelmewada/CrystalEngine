#pragma once

namespace CE::RPI
{
	class PassTree;
	class Scene;

	enum class PipelineViewType
	{
		Undefined = 0,
		Persistent,
		Transient
	};

	struct PipelineViews
	{
		PipelineViewTag viewTag{};
		PipelineViewType viewType{};

		Array<ViewPtr> views{};
	};

	using PipelineViewsByTag = HashMap<PipelineViewTag, PipelineViews>;

	class CORERPI_API RenderPipeline final
	{
		friend class RenderPipelineBuilder;
	public:

		virtual ~RenderPipeline();

		void SetPersistentView(const PipelineViewTag& viewTag, ViewPtr view);
		
	private:


	protected:

		RenderPipeline() = default;

		/// @brief The output render target of this render pipeline.
		RHI::RenderTarget* renderTarget = nullptr;

		/// @brief Name of the pipeline
		Name name{};

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
