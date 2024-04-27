#pragma once

#include "Core.h"
#include "CoreRHI.h"
#include "CoreShader.h"
#include "CoreMesh.h"

// ***********************************
// CoreRPI Includes

#if PAL_TRAIT_BUILD_EDITOR
namespace CE::Editor
{
	class FontAssetImportJob;
	class TextureAssetImportJob;
	class ShaderAssetImportJob;
}
#endif

namespace CE::RPI
{
	template<typename T>
    using Ptr = RHI::Ptr<T>;

	/// @brief Name tag used to identify each View inside a Render Pipeline.
	using SceneViewTag = CE::Name;

	using PipelineViewTagList = HashSet<SceneViewTag>;

	using InstanceBase = CE::IntrusiveBase;

	class View;
	class Scene;

	typedef Ptr<View> ViewPtr;

}

#include "RPILimits.h"

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
#include "RenderPipeline.h"

// Mesh
#include "VertexBuffer.h"
#include "Model/ModelLod.h"
#include "Model/ModelLodAsset.h"
#include "Model/ModelAsset.h"
#include "Model/Model.h"
#include "Model/MeshDrawPacket.h"

// Feature processors
#include "Feature/FeatureProcessor.h"
#include "Feature/FeatureProcessorRegistry.h"
#include "Feature/StaticMeshFeatureProcessor.h"
#include "Feature/LightFeatureProcessor.h"
#include "Feature/DirectionalLightFeatureProcessor.h"

#include "Scene.h"

#include "RPISystem.h"

#include "Texture.h"
#include "CubeMap/CubeMap.h"
#include "CubeMap/CubeMapProcessor.h"
#include "Material/MaterialProperty.h"
#include "Material/Material.h"
#include "TextureAsset.h"

#include "Shader/ShaderResourceGroup.h"
#include "Shader/ShaderVariant.h"
#include "Shader/Shader.h"
#include "Shader/ShaderCollection.h"

#include "Text/FontAtlasLayout.h"
#include "Text/FontAtlas.h"

#include "Renderer2D.h"

