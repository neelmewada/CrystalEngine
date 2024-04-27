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
			Array<Format> colorFormats{};
			Format depthFormat{};
			
			SceneViewTag viewTag{};
		};

		using PipelineStateList = Array<PipelineStateData>;

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

		void AddView(SceneViewTag viewTag, ViewPtr view);
		void RemoveView(SceneViewTag viewTag, ViewPtr view);

		void AddRenderPipeline(RenderPipeline* renderPipeline);
		void RemoveRenderPipeline(RenderPipeline* renderPipeline);

		u32 GetRenderPipelineCount() const { return renderPipelines.GetSize(); }

		RenderPipeline* GetRenderPipeline(u32 index) const { return renderPipelines[index]; }

		RHI::DrawListMask& GetDrawListMask(SceneViewTag viewTag);

		ArrayView<ViewPtr> GetViews(const SceneViewTag& viewTag);

		void Simulate(f32 currentTime);

		void PrepareRender(f32 currentTime, u32 imageIndex);

		void OnRenderEnd();

		void CollectDrawPackets();

		void RebuildPipelineLookupTable();
		
	private:

		bool needsLookupTableRebuild = true;

		HashMap<DrawListTag, PipelineStateList> pipelineLookupMap{};

		Array<RenderPipeline*> renderPipelines{};

		Array<FeatureProcessor*> featureProcessors{};

		FeatureProcessor::SimulatePacket simulatePacket{};
		FeatureProcessor::RenderPacket renderPacket{};

		/// @brief A Scene shader resource group (SRG_PerScene).
		RPI::ShaderResourceGroup* shaderResourceGroup = nullptr;

		SceneViewTag mainViewTag = "MainCamera";

		/// @brief A hash map of all views owned by this scene accessed by their respective tags.
		SceneViewsByTag viewsByTag{};

		friend class MeshDrawPacket;

#if PAL_TRAIT_BUILD_TESTS
		friend class ::RPI_Scene_Test;
#endif
	};
    
} // namespace CE::RPI
