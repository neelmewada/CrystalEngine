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

		RPI::Scene* scene = nullptr;

		MeshDrawPacketsByLod drawPacketsListByLod{};
	};

	using ModelHandle = PagedDynamicArray<ModelDataInstance>::Handle;

	struct ModelHandleDescriptor
	{
		Model* model = nullptr;
		ModelAsset* originalModel = nullptr;
	};

	CLASS()
	class CORERPI_API StaticMeshFeatureProcessor : public FeatureProcessor
	{
		CE_CLASS(StaticMeshFeatureProcessor, FeatureProcessor)
	public:

		StaticMeshFeatureProcessor();

		virtual ~StaticMeshFeatureProcessor();

		ModelHandle AcquireMesh(const ModelHandleDescriptor& modelHandleDescriptor);

		bool ReleaseMesh(ModelHandle& handle);

		void Simulate(const SimulatePacket& packet) override;

		void Render(const RenderPacket& packet) override;

		void OnRenderEnd() override;

	private:

		PagedDynamicArray<ModelDataInstance> modelInstances{};

	};

} // namespace CE::RPI

#include "StaticMeshFeatureProcessor.rtti.h"