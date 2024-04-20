#include "System.h"

namespace CE
{

    CE::RenderPipeline::RenderPipeline()
    {
        if (!IsDefaultInstance())
        {
	        renderPipeline = new RPI::RenderPipeline();
        }
    }

    CE::RenderPipeline::~RenderPipeline()
    {
        delete renderPipeline; renderPipeline = nullptr;
    }

    void CE::RenderPipeline::OnAfterConstruct()
    {
	    Super::OnAfterConstruct();

        this->Construct();
    }

    void CE::RenderPipeline::Construct()
    {
        
    }

} // namespace CE
