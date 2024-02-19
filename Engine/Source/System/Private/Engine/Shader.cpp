#include "System.h"

namespace CE
{

	void ShaderBlob::Release()
	{
		byteCode.Free();
	}

	void CE::ShaderVariant::Release()
	{
		
	}

	ShaderBlob* CE::ShaderVariant::GetShaderBlobForStage(ShaderStage stage)
	{
		for (int i = 0; i < shaderStageBlobs.GetSize(); i++)
		{
			if (shaderStageBlobs[i]->shaderStage == stage)
				return shaderStageBlobs[i];
		}

		return nullptr;
	}

	CE::Shader::Shader()
	{
		
	}

	CE::Shader::~Shader()
	{
		
	}

	CE::Shader* CE::Shader::GetErrorShader()
	{
		auto transient = ModuleManager::Get().GetLoadedModuleTransientPackage(MODULE_NAME);
		if (transient == nullptr)
		{
			CE_LOG(Error, All, "Cannot find transient package for module: {}", MODULE_NAME);
			return nullptr;
		}

		if (RHI::gDynamicRHI->GetGraphicsBackend() != RHI::GraphicsBackend::Vulkan)
		{
			return nullptr;
		}

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
		shader->passes.Add({});
		shader->passes[0].variants.Add({});

		CE::ShaderVariant& variant = shader->passes[0].variants[0];
		variant.shaderStageBlobs.Add(CreateObject<ShaderBlob>(shader, TEXT("VertexBlob")));
		variant.shaderStageBlobs.Top()->shaderStage = ShaderStage::Vertex;
		variant.shaderStageBlobs.Top()->byteCode.LoadData(vertSpv->GetData(), vertSpv->GetDataSize());

		variant.shaderStageBlobs.Add(CreateObject<ShaderBlob>(shader, TEXT("FragmentBlob")));
		variant.shaderStageBlobs.Top()->shaderStage = ShaderStage::Fragment;
		variant.shaderStageBlobs.Top()->byteCode.LoadData(fragSpv->GetData(), fragSpv->GetDataSize());

		vertSpv->Destroy();
		fragSpv->Destroy();

		return shader;
	}

} // namespace CE
