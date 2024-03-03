#include "CoreRPI.h"

#include <algorithm>

namespace CE::RPI
{
	CubeMap::CubeMap(const CubeMapDescriptor& desc) : desc(desc)
	{
		
	}

	CubeMap::~CubeMap()
	{
		delete cubeMap; cubeMap = nullptr;
		delete diffuseIrradiance; diffuseIrradiance = nullptr;
	}

} // namespace CE::RPI
