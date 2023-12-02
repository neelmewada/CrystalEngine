#include "System.h"

namespace CE
{

    Material::Material()
    {
		
    }

	Material* Material::GetErrorMaterial()
	{
		auto transient = ModuleManager::Get().GetLoadedModuleTransientPackage(MODULE_NAME);
		if (transient == nullptr)
		{
			CE_LOG(Error, All, "Cannot find transient package for module: {}", MODULE_NAME);
			return nullptr;
		}

		if (RHI::gDynamicRHI->GetGraphicsBackend() != RHI::GraphicsBackend::Vulkan)
			return nullptr;

		Shader* errorShader = Shader::GetErrorShader();
		if (errorShader == nullptr)
			return nullptr;

		for (auto object : transient->GetSubObjectMap())
		{
			if (object != nullptr && object->IsOfType<Material>() && object->GetName() == "ErrorMaterial")
			{
				Material* mat = Object::CastTo<Material>(object);
				if (mat->shader == errorShader)
					return mat;
			}
		}

		Material* errorMaterial = CreateObject<Material>(transient, "ErrorMaterial", OF_Transient);
		errorMaterial->shader = errorShader;

		return errorMaterial;
	}

} // namespace CE
