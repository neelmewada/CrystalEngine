#pragma once

namespace CE::RPI
{
	class Shader;

	struct CubeMapDescriptor
	{
		RPI::Texture* sourceImage = nullptr;
		RPI::Shader* equirectangularShader = nullptr;
		RHI::Format preferredFormat = RHI::Format::B8G8R8A8_UNORM;
		// Use 0 to auto choose resolution based on source image
		u32 cubeMapResolution = 0;
		bool diffuseIrradiance = false;
	};

	class CORERPI_API CubeMap
	{
	public:
		CubeMap(const CubeMapDescriptor& desc);

		virtual ~CubeMap();

	private:

		CubeMapDescriptor desc{};

		RPI::Texture* cubeMap = nullptr;
		RPI::Texture* diffuseIrradiance = nullptr;

	};

} // namespace CE::RPI
