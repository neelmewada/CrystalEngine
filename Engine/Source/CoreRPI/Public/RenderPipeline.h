#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RenderPipeline_DefaultPipelineTree_Test;
#endif

namespace CE::RPI
{
	class PassTree;
	class Scene;
	class View;

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

		RHI::DrawListMask drawListMask{};
	};

	using PipelineViewsByTag = HashMap<PipelineViewTag, PipelineViews>;
	
	class CORERPI_API RenderPipeline
	{
	public:

		RenderPipeline();

		virtual ~RenderPipeline();
        
        PassTree* GetPassTree() const
        {
            return passTree;
        }

		Ptr<PassAttachment> FindAttachment(Name name);

		PassAttachment* AddAttachment(const RPI::PassImageAttachmentDesc& imageDesc);
		PassAttachment* AddAttachment(const RPI::PassBufferAttachmentDesc& bufferDesc);

		/// @brief Name of the pipeline
		Name name{};

		/// @brief Name of the pass definition to instantiate as root pass
		Name rootPassDefinitionName{};

		/// @brief Name tag of the main view.
		PipelineViewTag mainViewTag = "MainCamera";

		/// @brief Scene this render pipeline belongs to.
		Scene* scene = nullptr;

		/// @brief The pass tree hierarchy. Pipeline owns & manages the passes.
		PassTree* passTree = nullptr;

		Array<Ptr<PassAttachment>> attachments{};

#if PAL_TRAIT_BUILD_TESTS
		friend class RenderPipeline_DefaultPipelineTree_Test;
#endif

		friend class RenderPipelineBuilder;
	};
    
} // namespace CE::RPI
