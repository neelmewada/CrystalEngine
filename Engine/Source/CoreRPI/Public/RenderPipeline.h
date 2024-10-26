#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RenderPipeline_DefaultPipelineTree_Test;
#endif

namespace CE::RPI
{
	class PassTree;
	class Scene;
	class View;
	class ShaderCollection;

	struct SceneViews
	{
		SceneViewTag viewTag{};

		Array<ViewPtr> views{};

		RHI::DrawListMask drawListMask{};
	};

	using SceneViewsByTag = HashMap<SceneViewTag, SceneViews>;
	
	class CORERPI_API RenderPipeline final
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

		void ImportScopeProducers(RHI::FrameScheduler* scheduler);

		void ApplyShaderLayout(RPI::ShaderCollection* shaderCollection);

		/// @brief Name of the pipeline
		Name name{};

		/// @brief Unique id of the pipeline
		Uuid uuid{};

		/// @brief Name tag of the main view.
		SceneViewTag mainViewTag = "MainCamera";

		/// @brief Scene this render pipeline belongs to.
		Scene* scene = nullptr;

		ViewPtr view = nullptr;

		/// @brief The pass tree hierarchy. Pipeline owns & manages the passes.
		PassTree* passTree = nullptr;

		Array<Ptr<PassAttachment>> attachments{};

#if PAL_TRAIT_BUILD_TESTS
		friend class RenderPipeline_DefaultPipelineTree_Test;
#endif

		friend class RenderPipelineBuilder;
	};
    
} // namespace CE::RPI
