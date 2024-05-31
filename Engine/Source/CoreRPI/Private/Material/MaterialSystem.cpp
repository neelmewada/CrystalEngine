#include "CoreRPI.h"

namespace CE::RPI
{

	MaterialSystem& MaterialSystem::Get()
	{
		static MaterialSystem instance{};
		return instance;
	}

	void MaterialSystem::Update(u32 imageIndex)
	{
		LockGuard lock{ materialsMutex };

		for (RPI::Material* material : materials)
		{
			material->FlushProperties(imageIndex);
		}
	}

	void MaterialSystem::AddMaterial(RPI::Material* material)
	{
		LockGuard lock{ materialsMutex };

		materials.Add(material);
	}

	void MaterialSystem::RemoveMaterial(RPI::Material* material)
	{
		LockGuard lock{ materialsMutex };

		materials.Remove(material);
	}

} // namespace CE::RPI
