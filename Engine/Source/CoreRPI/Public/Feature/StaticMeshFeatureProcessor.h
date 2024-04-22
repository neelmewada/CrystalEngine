#pragma once

namespace CE::RPI
{
	class Model;
	class ModelAsset;
	class ModelLod;

	class CORERPI_API ModelDataInstance
	{
	public:

		Model* model = nullptr;

		ModelAsset* originalModel = nullptr;

		Matrix4x4 worldTransform{};
	};

	CLASS()
	class CORERPI_API StaticMeshFeatureProcessor : public FeatureProcessor
	{
		CE_CLASS(StaticMeshFeatureProcessor, FeatureProcessor)
	public:

		StaticMeshFeatureProcessor();

		virtual ~StaticMeshFeatureProcessor();

		void Simulate(const SimulatePacket& packet) override;

		void Render(const RenderPacket& packet) override;

		void OnRenderEnd() override;

	private:

		Array<ModelDataInstance> models{};

	};

} // namespace CE::RPI

#include "StaticMeshFeatureProcessor.rtti.h"