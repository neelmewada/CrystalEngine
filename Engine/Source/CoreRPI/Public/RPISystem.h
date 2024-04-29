#pragma once

namespace CE::RPI
{
	class Texture;

	enum class BuiltinDrawItemTag
	{
		None = 0,
		Depth,
		Opaque,
		Shadow,
		Skybox,
		UI,
		Transparent,
		COUNT
	};
	ENUM_CLASS(BuiltinDrawItemTag);

	/// @brief RPISystem owns and manages all scenes.
	class CORERPI_API RPISystem final
	{
		CE_NO_COPY(RPISystem)
	public:

		static RPISystem& Get();

		RHI::DrawListTagRegistry* GetDrawListTagRegistry()
		{
			return rhiSystem.GetDrawListTagRegistry();
		}

		void Initialize();

		void Shutdown();

		void SimulationTick(u32 imageIndex);
		void RenderTick(u32 imageIndex);

		f32 GetCurrentTime() const;

		RPI::Texture* GetDefaultAlbedoTex() const
		{
			return defaultAlbedoTex;
		}

		RPI::Texture* GetDefaultRoughnessTex() const
		{
			return defaultRoughnessTex;
		}

		RPI::Texture* GetDefaultNormalTex() const
		{
			return defaultNormalTex;
		}

		RHI::Sampler* FindOrCreateSampler(const RHI::SamplerDescriptor& desc);

		const Array<RHI::VertexBufferView>& GetFullScreenQuad() const { return quadVertexBufferViews; }
		RHI::DrawLinearArguments GetFullScreenQuadDrawArgs() const { return quadDrawArgs; }

		const Array<RHI::VertexBufferView>& GetTextQuad() const { return textQuadVertexBufferViews; }
		RHI::DrawLinearArguments GetTextQuadDrawArgs() const { return textQuadDrawArgs; }

		DrawListTag GetBuiltinDrawListTag(BuiltinDrawItemTag buitinTag) { return builtinDrawTags[buitinTag]; }

	private:

		RPISystem() = default;

		void CreateDefaultTextures();
		void CreateFullScreenQuad();

		RHI::RHISystem rhiSystem{};
		Array<RHI::Buffer*> vertexBuffers{};
		Array<RHI::VertexBufferView> quadVertexBufferViews{};
		RHI::DrawLinearArguments quadDrawArgs{};

		Array<RHI::VertexBufferView> textQuadVertexBufferViews{};
		RHI::DrawLinearArguments textQuadDrawArgs{};

		Array<Scene*> scenes{};

		RPI::Texture* defaultAlbedoTex = nullptr;
		RPI::Texture* defaultNormalTex = nullptr;
		RPI::Texture* defaultRoughnessTex = nullptr;

		SharedMutex samplerCacheMutex{};
		HashMap<SIZE_T, RHI::Sampler*> samplerCache{};

		HashMap<BuiltinDrawItemTag, RHI::DrawListTag> builtinDrawTags{};

		b8 isFirstTick = true;
		clock_t startTime;
		f32 currentTime = 0.0f;

		friend class RPI::Scene;
	};
    
} // namespace CE::RPI
