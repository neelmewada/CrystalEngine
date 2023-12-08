#include "System.h"

namespace CE
{
	void ShaderBlob::Release()
	{
		byteCode.Free();
	}

	void ShaderVariant::Release()
	{
		
	}

	ShaderBlob* ShaderVariant::GetShaderBlobForStage(ShaderStage stage)
	{
		for (int i = 0; i < shaderStageBlobs.GetSize(); i++)
		{
			if (shaderStageBlobs[i].shaderStage == stage)
				return &shaderStageBlobs[i];
		}

		return nullptr;
	}

	Shader::Shader()
	{
		
	}

	Shader::~Shader()
	{
		for (auto module : allModules)
		{
			RHI::gDynamicRHI->DestroyShaderModule(module.vertex);
			RHI::gDynamicRHI->DestroyShaderModule(module.fragment);
		}

		allModules.Clear();
	}

	Shader* Shader::GetErrorShader()
	{
		auto transient = ModuleManager::Get().GetLoadedModuleTransientPackage(MODULE_NAME);
		if (transient == nullptr)
		{
			CE_LOG(Error, All, "Cannot find transient package for module: {}", MODULE_NAME);
			return nullptr;
		}

		if (RHI::gDynamicRHI->GetGraphicsBackend() != RHI::GraphicsBackend::Vulkan)
			return nullptr;

		for (auto object : transient->GetSubObjectMap())
		{
			if (object != nullptr && object->IsOfType<Shader>() && object->GetName() == "ErrorShader")
			{
				return Object::CastTo<Shader>(object);
			}
		}
		
		Resource* vertSpv = GetResourceManager()->LoadResource("/System/Resources/TestShaderVert.spv", transient);
		Resource* fragSpv = GetResourceManager()->LoadResource("/System/Resources/TestShaderFrag.spv", transient);

		Shader* shader = CreateObject<Shader>(transient, "ErrorShader", OF_Transient);
		shader->passes.Add(CreateObject<ShaderPass>(shader, TEXT("Main")));
		shader->passes[0]->variants.Add({});

		ShaderVariant& variant = shader->passes[0]->variants[0];
		variant.shaderStageBlobs.Add(ShaderBlob{});
		variant.shaderStageBlobs.Top().shaderStage = ShaderStage::Vertex;
		variant.shaderStageBlobs.Top().byteCode.LoadData(vertSpv->GetData(), vertSpv->GetDataSize());

		variant.shaderStageBlobs.Add(ShaderBlob{});
		variant.shaderStageBlobs.Top().shaderStage = ShaderStage::Fragment;
		variant.shaderStageBlobs.Top().byteCode.LoadData(fragSpv->GetData(), fragSpv->GetDataSize());

		return shader;
	}

} // namespace CE
