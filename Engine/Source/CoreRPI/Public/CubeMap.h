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

	struct CubeMapProcessInfo
	{
		Name name{};
		CMImage sourceImage{};
		RPI::Shader* equirectangularShader = nullptr;
		RPI::Shader* grayscaleShader = nullptr;
		RPI::Shader* rowAverageShader = nullptr;
		RPI::Shader* columnAverageShader = nullptr;
		RPI::Shader* divisionShader = nullptr;
		RPI::Shader* cdfConditionalInverseShader = nullptr;
		RPI::Shader* cdfMarginalInverseShader = nullptr;

		// The preferred color format for the cubemap. Do NOT use compressed format here!
		RHI::Format preferredFormat = RHI::Format::R16G16B16A16_SFLOAT;
		HdrEncoding hdrEncoding = HdrEncoding::None;

		// Use 0 to auto choose resolution based on source image
		u32 cubeMapResolution = 0;

		// Set to 0 to disable irradiance computation
		u32 diffuseIrradianceResolution = 32;
	};

	class CORERPI_API CubeMap
	{
	private:
		CubeMap() = default;

	public:
		CubeMap(const CubeMapDescriptor& desc);

		virtual ~CubeMap();

		// Takes an equirectangular HDRI texture and generates a cubemap out of it, and optionally generate IBL maps.
		static CubeMap* ProcessCubeMap(const CubeMapProcessInfo& processInfo);

		inline RPI::Texture* GetCubeMap() const { return cubeMap; }
		inline RPI::Texture* GetDiffuseIrradiance() const { return diffuseIrradiance; }

	private:

		CubeMapDescriptor desc{};

		RPI::Texture* cubeMap = nullptr;
		RPI::Texture* diffuseIrradiance = nullptr;

	};

} // namespace CE::RPI

#include "CubeMap.rtti.h"
