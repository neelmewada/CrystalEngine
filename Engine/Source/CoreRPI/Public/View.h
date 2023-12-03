#pragma once

namespace CE::RPI
{
    
	class CORERPI_API View final
	{
	public:

		View();
		~View();

		enum UsageFlags
		{
			UsageNone = 0,
			UsageCamera = BIT(0),
			UsageShadow = BIT(1),
			UsageReflectiveCubeMap = BIT(2)
		};

	private:

		RPI::RenderViewport* targetViewport = nullptr;

		RPI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		UsageFlags usageFlags = UsageCamera;
	};

} // namespace CE::RPI
