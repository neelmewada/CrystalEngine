#include "System.h"

namespace CE
{

	ShaderBlob::~ShaderBlob()
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
		for (auto rpiShader : rpiShaderPerPass)
		{
			delete rpiShader;
		}
		rpiShaderPerPass.Clear();
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

	RPI::Shader* CE::Shader::GetOrCreateRPIShader(int passIndex)
	{
		if (rpiShaderPerPass.GetSize() == 0)
		{
			for (int i = 0; i < passes.GetSize(); i++)
			{
				RPI::Shader* rpiShader = new RPI::Shader();

				for (int j = 0; j < passes[i].variants.GetSize(); j++)
				{
					const CE::ShaderVariant& variant = passes[i].variants[j];
					
					RPI::ShaderVariantDescriptor2 variantDesc{};
					variantDesc.preprocessData = preprocessData;
					variantDesc.reflectionInfo = variant.reflectionInfo;
					variantDesc.passIndex = i;
					variantDesc.subShaderIndex = 0;

					for (ShaderBlob* curShaderBlob : variant.shaderStageBlobs)
					{
						variantDesc.moduleDesc.Add({});
						variantDesc.moduleDesc.Top().byteCode = curShaderBlob->byteCode.GetDataPtr();
						variantDesc.moduleDesc.Top().byteSize = curShaderBlob->byteCode.GetDataSize();
						variantDesc.moduleDesc.Top().name = curShaderBlob->GetName().GetString();
						variantDesc.moduleDesc.Top().stage = curShaderBlob->shaderStage;
					}
					
					rpiShader->AddVariant(variantDesc);
				}

				rpiShaderPerPass.Add(rpiShader);
			}
		}

		return rpiShaderPerPass[passIndex];
	}

} // namespace CE
