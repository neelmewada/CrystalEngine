#pragma once

namespace CE::RPI
{
	class Model;
	class ModelAsset;
	class ModelLod;
	class StaticMeshFeatureProcessor;

	class CORERPI_API ModelDataInstance
	{
	public:

		Model* model = nullptr;

		ModelAsset* originalModel = nullptr;

		Matrix4x4 worldTransform{};

		RPI::Scene* scene = nullptr;

		RPI::Material* material = nullptr;

		MeshDrawPacketsByLod drawPacketsListByLod{};

		Array<RHI::ShaderResourceGroup*> objectSrgList{};

		void Init(StaticMeshFeatureProcessor* fp);
		void Deinit(StaticMeshFeatureProcessor* fp);
		void BuildDrawPacketList(StaticMeshFeatureProcessor* fp, u32 modelLodIndex);
		void UpdateDrawPackets(StaticMeshFeatureProcessor* fp);

		struct Flags
		{
			bool isInitialized : 1 = false;
		} flags{};
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