#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RPI_Scene_Test;
#endif

namespace CE::RPI
{
	class FeatureProcessor;

	class CORERPI_API Scene final
	{
	public:

		struct PipelineStateData
		{
			RHI::MultisampleState multisampleState{};
			Array<RHI::Format> colorFormats{};
		};

		Scene();
		~Scene();

		FeatureProcessor* AddFeatureProcessor(SubClass<FeatureProcessor> classType);

		FeatureProcessor* GetFeatureProcessor(SubClass<FeatureProcessor> classType);

		template<typename TFeatureProcessor> requires TIsBaseClassOf<FeatureProcessor, TFeatureProcessor>::Value
		TFeatureProcessor* AddFeatureProcessor()
		{
			return (TFeatureProcessor*)AddFeatureProcessor(TFeatureProcessor::Type());
		}

		template<typename TFeatureProcessor> requires TIsBaseClassOf<FeatureProcessor, TFeatureProcessor>::Value
		TFeatureProcessor* GetFeatureProcessor()
		{
			return (TFeatureProcessor*)GetFeatureProcessor(TFeatureProcessor::Type());
		}

		void AddView(PipelineViewTag viewTag, ViewPtr view);
		void RemoveView(PipelineViewTag viewTag, ViewPtr view);

		void AddRenderPipeline(RenderPipeline* renderPipeline);

		u32 GetRenderPipelineCount() const { return renderPipelines.GetSize(); }

		RenderPipeline* GetRenderPipeline(u32 index) const { return renderPipelines[index]; }

		RHI::DrawListMask& GetDrawListMask(PipelineViewTag viewTag);

		void Simulate(f32 currentTime);

		void PrepareRender(f32 currentTime, u32 imageIndex);

		void OnRenderEnd();

		void CollectDrawPackets();

	private:

		Array<RenderPipeline*> renderPipelines{};

		Array<FeatureProcessor*> featureProcessors{};

		FeatureProcessor::SimulatePacket simulatePacket{};
		FeatureProcessor::RenderPacket renderPacket{};

		/// @brief A Scene shader resource group (SRG_PerScene).
		RPI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		PipelineViewTag mainViewTag = "MainCamera";

		/// @brief A hash map of all views owned by this scene accessed by their respective tags.
		PipelineViewsByTag viewsByTag{};

		friend class MeshDrawPacket;

#if PAL_TRAIT_BUILD_TESTS
		friend class ::RPI_Scene_Test;
#endif
	};
    
} // namespace CE::RPI
