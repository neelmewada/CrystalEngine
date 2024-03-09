#pragma once

#include "CoreMinimal.h"
#include "CoreRHI.h"
#include "CoreShader.h"
#include "CoreMesh.h"

// ***********************************
// CoreRPI Includes

namespace CE::RPI
{
	template<typename T>
    using Ptr = RHI::Ptr<T>;

	/// @brief Name tag used to identify each View inside a Render Pipeline.
	using PipelineViewTag = CE::Name;

	using PipelineViewTagList = HashSet<PipelineViewTag>;

	using InstanceBase = CE::IntrusiveBase;


}

// Passes
#include "Pass/PassAttachmentDefines.h"
#include "Pass/PassAttachment.h"
#include "Pass/Pass.h"
#include "Pass/ParentPass.h"
#include "Pass/GpuPass.h"
#include "Pass/RasterPass.h"
#include "Pass/PassTree.h"
#include "Pass/PassRegistry.h"
#include "Pass/PassSystem.h"

#include "View.h"
#include "RenderPipelineBuilder.h"
#include "RenderPipeline.h"
#include "Scene.h"

#include "RPISystem.h"

#include "Texture.h"
#include "CubeMap/CubeMap.h"
#include "CubeMap/CubeMapProcessor.h"
#include "Material/MaterialProperty.h"
#include "Material/Material.h"

#include "VertexBuffer.h"
#include "Model/ModelLod.h"
#include "Model/ModelLodAsset.h"
#include "Model/ModelAsset.h"
#include "Model/Model.h"

#include "Shader/GraphicsPipelineCollection.h"
#include "Shader/ShaderResourceGroup.h"
#include "Shader/ShaderVariant.h"
#include "Shader/Shader.h"

#include "Feature/FeatureProcessor.h"
#include "Feature/FeatureProcessorRegistry.h"

