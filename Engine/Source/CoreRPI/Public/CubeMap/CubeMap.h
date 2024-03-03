#pragma once

namespace CE::RPI
{
	class Shader;

	ENUM()
	enum class HdrEncoding
	{
		None = 0,
	};
	ENUM_CLASS(HdrEncoding);

	struct CubeMapDescriptor
	{

	};

	class CORERPI_API CubeMap
	{
	private:
		CubeMap() = default;

	public:
		CubeMap(const CubeMapDescriptor& desc);

		virtual ~CubeMap();

		inline RPI::Texture* GetCubeMap() const { return cubeMap; }
		inline RPI::Texture* GetDiffuseIrradiance() const { return diffuseIrradiance; }

	private:

		CubeMapDescriptor desc{};

		RPI::Texture* cubeMap = nullptr;
		RPI::Texture* diffuseIrradiance = nullptr;

	};

} // namespace CE::RPI

#include "CubeMap.rtti.h"
