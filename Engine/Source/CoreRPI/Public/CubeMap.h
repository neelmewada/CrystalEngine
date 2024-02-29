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

		HdrEncoding hdrEncoding = HdrEncoding::None;
		bool useCompression = false;

		// Use 0 to auto choose resolution based on source image
		u32 cubeMapResolution = 0;

		// Set to 0 to disable irradiance computation
		u32 diffuseIrradianceResolution = 32;
	};

	class CORERPI_API CubeMapProcessor
	{
	public:

		CubeMapProcessor() = default;
		virtual ~CubeMapProcessor() = default;

		// Takes an equirectangular HDRI texture and generates a cubemap out of it, and optionally generate IBL maps.
		void ProcessCubeMapOffline(const CubeMapProcessInfo& processInfo, BinaryBlob& cubeMapOutput);

	private:

	};

} // namespace CE::RPI

#include "CubeMap.rtti.h"
