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

		Array<View*> views{};

		RHI::DrawListMask drawListMask{};
	};

	using PipelineViewsByTag = HashMap<PipelineViewTag, PipelineViews>;

	class CORERPI_API RenderPipeline
	{
		friend class RenderPipelineBuilder;
	public:

		virtual ~RenderPipeline();
        
        inline PassTree* GetPassTree() const
        {
            return passTree;
        }

	private:

	protected:

		RenderPipeline();

		/// @brief Name of the pipeline
		Name name{};

		/// @brief Name of the pass definition to instantiate as root pass
		Name rootPassDefinitionName{};

		/// @brief Name tag of the main view.
		Name mainViewTag = "MainCamera";

		/// @brief Scene this render pipeline belongs to.
		Scene* scene = nullptr;

		/// @brief The pass tree hierarchy. Pipeline owns & manages the passes.
		PassTree* passTree = nullptr;

		/// @brief A hash map of all views owned by this pipeline accessed by their respective tags
		PipelineViewsByTag pipelineViewsByTag{};

#if PAL_TRAIT_BUILD_TESTS
		friend class RenderPipeline_DefaultPipelineTree_Test;
#endif
	};
    
} // namespace CE::RPI

#include "RenderPipeline.rtti.h"