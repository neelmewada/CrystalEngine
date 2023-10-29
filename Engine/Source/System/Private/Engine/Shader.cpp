#include "System.h"

namespace CE
{
	void ShaderBlob::Release()
	{
		source.Free();
	}

	void ShaderVariant::Release()
	{
		
	}

	Shader::Shader()
	{
		
	}

	Shader::~Shader()
	{
		for (auto module : shaderModules)
		{
			RHI::gDynamicRHI->DestroyShaderModule(module);
		}
		shaderModules.Clear();
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
        
		Resource* vertSpv = GetResourceManager()->LoadResource("ErrorShaderVert.spv", transient);
		Resource* fragSpv = GetResourceManager()->LoadResource("ErrorShaderFrag.spv", transient);

		Shader* shader = CreateObject<Shader>(transient, "ErrorShader", OF_Transient);
		shader->variants.Resize(1);

		ShaderVariant& variant = shader->variants[0];
		variant.vertexShader.shaderStage = ShaderStage::Vertex;
		variant.vertexShader.source.LoadData(vertSpv->GetData(), vertSpv->GetDataSize());

		variant.fragmentShader.shaderStage = ShaderStage::Fragment;
		variant.fragmentShader.source.LoadData(fragSpv->GetData(), fragSpv->GetDataSize());

		return shader;
	}

	const Array<RHI::ShaderModule*>& Shader::GetShaderModules()
	{
		if (shaderModules.IsEmpty() && variants.NonEmpty())
		{
			RHI::ShaderModuleDesc vertDesc{};
			vertDesc.byteCode = variants[0].vertexShader.source.GetDataPtr();
			vertDesc.byteSize = variants[0].vertexShader.source.GetDataSize();
			auto vertShaderModule = RHI::gDynamicRHI->CreateShaderModule(vertDesc);

			RHI::ShaderModuleDesc fragDesc{};
			fragDesc.byteCode = variants[0].vertexShader.source.GetDataPtr();
			fragDesc.byteSize = variants[0].vertexShader.source.GetDataSize();
			auto fragShaderModule = RHI::gDynamicRHI->CreateShaderModule(fragDesc);

			shaderModules.Add(vertShaderModule);
			shaderModules.Add(fragShaderModule);
		}

		return shaderModules;
	}

} // namespace CE
