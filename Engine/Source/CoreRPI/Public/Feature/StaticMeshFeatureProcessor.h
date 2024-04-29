#pragma once

namespace CE::RPI
{
	class Model;
	class ModelAsset;
	class ModelLod;
	class StaticMeshFeatureProcessor;
	class Material;

	class CORERPI_API ModelDataInstance
	{
	public:

		Model* model = nullptr;

		ModelAsset* originalModel = nullptr;

		Matrix4x4 worldTransform{};

		RPI::Scene* scene = nullptr;

		CustomMaterialMap materialMap{};

		MeshDrawPacketsByLod drawPacketsListByLod{};

		Array<RHI::ShaderResourceGroup*> objectSrgList{};

		void Init(StaticMeshFeatureProcessor* fp);
		void Deinit(StaticMeshFeatureProcessor* fp);
		void BuildDrawPacketList(StaticMeshFeatureProcessor* fp, u32 modelLodIndex);
		void UpdateDrawPackets(StaticMeshFeatureProcessor* fp, bool forceUpdate = false);

		struct Flags
		{
			bool visible : 1 = true;
			bool initialized : 1 = false;
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

		ModelHandle AcquireMesh(const ModelHandleDescriptor& modelHandleDescriptor, const CustomMaterialMap& materialMap);
		ModelHandle AcquireMesh(const ModelHandleDescriptor& modelHandleDescriptor, RPI::Material* defaultMaterial);
		bool ReleaseMesh(ModelHandle& handle);

		void Simulate(const SimulatePacket& packet) override;

		void Render(const RenderPacket& packet) override;

		void OnRenderEnd() override;

	private:

		Array<Job*> CreateInitJobs();

		PagedDynamicArray<ModelDataInstance> modelInstances{};

		bool forceRebuildDrawPackets = false;
	};

} // namespace CE::RPI

#include "StaticMeshFeatureProcessor.rtti.h"