#include "System.h"

namespace CE
{

	RenderPipelineAsset::RenderPipelineAsset()
	{
		
	}

	RenderPipelineAsset::~RenderPipelineAsset()
	{
		
	}
	
	void RenderPipelineAsset::ApplyChanges()
	{
		for (CE::RenderPipeline* renderPipeline : renderPipelines)
		{
			renderPipeline->MarkDirty();
		}
	}

} // namespace CE
