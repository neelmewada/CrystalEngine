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
			
		}
	}

	Shader* Shader::GetDebugShader()
	{
		auto transient = ModuleManager::Get().GetLoadedModuleTransientPackage(MODULE_NAME);
		if (transient == nullptr)
		{
			CE_LOG(Error, All, "Cannot find transient package for module: {}", MODULE_NAME);
			return nullptr;
		}

		return nullptr;
	}

} // namespace CE
