#pragma once

namespace CE::RPI
{
    struct alignas(16) DirectionalLightData
    {
        Matrix4x4 lightSpaceMatrix;
        Vec4 direction;
        Vec4 colorAndIntensity;
        float temperature;
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