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

	GPUShaderModule* Shader::FindOrCreateModule(Name passName, SIZE_T variantHash)
	{
		auto module = FindModule(passName, variantHash);
		if (module != nullptr)
			return module;

		for (auto pass : passes)
		{
			if (!passName.IsValid() || pass->passName == passName)
			{
				for (auto& variant : pass->variants)
				{
					if (variantHash == 0 || variant.variantHash == variantHash)
					{
						allModules.Add({});
						module = &allModules.Top();
						module->passName = passName;
						module->variantHash = variant.variantHash;

						RHI::ShaderModuleDesc vertexDesc{};
						vertexDesc.byteCode = variant.GetShaderBlobForStage(ShaderStage::Vertex)->byteCode.GetDataPtr();
						vertexDesc.byteSize = variant.GetShaderBlobForStage(ShaderStage::Vertex)->byteCode.GetDataSize();
						vertexDesc.name = "Vertex";
						if (preprocessData != nullptr)
							vertexDesc.name = preprocessData->shaderName.GetString();
						module->vertex = RHI::gDynamicRHI->CreateShaderModule(vertexDesc);

						RHI::ShaderModuleDesc fragmentDesc{};
						fragmentDesc.byteCode = variant.GetShaderBlobForStage(ShaderStage::Fragment)->byteCode.GetDataPtr();
						fragmentDesc.byteSize = variant.GetShaderBlobForStage(ShaderStage::Fragment)->byteCode.GetDataSize();
						fragmentDesc.name = "Fragment";
						if (preprocessData != nullptr)
							fragmentDesc.name = preprocessData->shaderName.GetString();
						module->fragment = RHI::gDynamicRHI->CreateShaderModule(fragmentDesc);

						return module;
					}
				}
			}
		}

		return nullptr;
	}

	GPUShaderModule* Shader::FindModule(Name passName, SIZE_T variantHash)
	{
		for (int i = 0; i < allModules.GetSize(); i++)
		{
			const auto& module = allModules[i];
			if (!passName.IsValid())
				return &allModules[i];
			if (module.passName == passName && (variantHash == 0 || module.variantHash == variantHash))
				return &allModules[i];
		}

		return nullptr;
	}

} // namespace CE
