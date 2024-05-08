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
		for (RPI::Material* material : materials)
		{
			material->FlushProperties(imageIndex);
		}
	}

} // namespace CE::RPI
