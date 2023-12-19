#pragma once

#include "CoreMinimal.h"
#include "CoreRHI.h"
#include "CoreShader.h"

// ***********************************
// CoreRPI Includes

namespace CE::RPI
{
	template<typename T>
	using Ptr = RHI::Ptr<T>;
}

// Passes
#include "Pass/PassAttachment.h"
#include "Pass/Pass.h"
#include "Pass/ParentPass.h"
#include "Pass/PassFactory.h"
#include "Pass/PassTemplate.h"
#include "Pass/PassLibrary.h"
#include "Pass/PassSystem.h"

#include "RenderTarget.h"
#include "View.h"
#include "RenderPipeline.h"
#include "Scene.h"

#include "RPISystem.h"

#include "Material/Material.h"

#include "Model/ModelLod.h"
#include "Model/Model.h"

#include "Shader/ShaderResourceGroup.h"
#include "Shader/ShaderVariant.h"
#include "Shader/Shader.h"

#include "Feature/FeatureProcessor.h"
#include "Feature/FeatureProcessorRegistry.h"


