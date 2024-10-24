#pragma once

namespace CE::RPI
{
	class DirectionalLightFeatureProcessor;

    struct alignas(16) DirectionalLightConstants
    {
        Matrix4x4 lightSpaceMatrix;
        Vec4 direction;
        Vec4 colorAndIntensity;
        float temperature;
    };

	class CORERPI_API DirectionalLightInstance
	{
	public:

		Model* model = nullptr;

		ModelAsset* originalModel = nullptr;

		Matrix4x4 localToWorldTransform{};

		RPI::Scene* scene = nullptr;

		CustomMaterialMap materialMap{};

		MeshDrawPacketsByLod drawPacketsListByLod{};

		Array<RHI::ShaderResourceGroup*> objectSrgList{};

		StaticArray<RHI::Buffer*, RHI::Limits::MaxSwapChainImageCount> objectBuffers{};

		void Init(DirectionalLightFeatureProcessor* fp);
		void Deinit(DirectionalLightFeatureProcessor* fp);

		void UpdateSrgs(int imageIndex);

		struct Flags
		{
			bool visible : 1 = true;
			bool initialized : 1 = false;
		} flags{};
	};

    CLASS()
    class CORERPI_API DirectionalLightFeatureProcessor : public LightFeatureProcessor
    {
        CE_CLASS(DirectionalLightFeatureProcessor, LightFeatureProcessor)
    public:

        DirectionalLightFeatureProcessor();

        virtual ~DirectionalLightFeatureProcessor();

    protected:


    };
    
} // namespace CE::RPI

#include "DirectionalLightFeatureProcessor.rtti.h"