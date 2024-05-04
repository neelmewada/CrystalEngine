#pragma once

namespace CE::RPI
{
	class Texture;
	class ShaderCollection;

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

	struct RPISystemInitInfo
	{
		RPI::ShaderCollection* standardShader = nullptr;
		RPI::ShaderCollection* iblConvolutionShader = nullptr;
		
	};

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
		void PostInitialize(const RPISystemInitInfo& initInfo);

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

		RPI::Texture* GetBrdfLutTexture() const
		{
			return brdfLutTexture;
		}

		RHI::Sampler* FindOrCreateSampler(const RHI::SamplerDescriptor& desc);

		const Array<RHI::VertexBufferView>& GetFullScreenQuad() const { return quadVertexBufferViews; }
		RHI::DrawLinearArguments GetFullScreenQuadDrawArgs() const { return quadDrawArgs; }

		const Array<RHI::VertexBufferView>& GetTextQuad() const { return textQuadVertexBufferViews; }
		RHI::DrawLinearArguments GetTextQuadDrawArgs() const { return textQuadDrawArgs; }

		DrawListTag GetBuiltinDrawListTag(BuiltinDrawItemTag buitinTag) { return builtinDrawTags[buitinTag]; }

		const auto& GetViewSrgLayout() const { return viewSrgLayout; }
		const auto& GetSceneSrgLayout() const { return sceneSrgLayout; }

	private:

		RPISystem() = default;

		void CreateDefaultTextures();
		void CreateFullScreenQuad();

		void CreateBrdfLutTexture();

		bool isInitialized = false;
		
		RHI::RHISystem rhiSystem{};
		Array<RHI::Buffer*> vertexBuffers{};
		Array<RHI::VertexBufferView> quadVertexBufferViews{};
		RHI::DrawLinearArguments quadDrawArgs{};

		Array<RHI::VertexBufferView> textQuadVertexBufferViews{};
		RHI::DrawLinearArguments textQuadDrawArgs{};

		Array<Scene*> scenes{};

		RHI::ShaderResourceGroupLayout sceneSrgLayout{};
		RHI::ShaderResourceGroupLayout viewSrgLayout{};

		RPI::ShaderCollection* standardShader = nullptr;
		RPI::ShaderCollection* iblConvolutionShader = nullptr;

		RPI::Texture* defaultAlbedoTex = nullptr;
		RPI::Texture* defaultNormalTex = nullptr;
		RPI::Texture* defaultRoughnessTex = nullptr;

		RPI::Texture* brdfLutTexture = nullptr;

		SharedMutex samplerCacheMutex{};
		HashMap<SIZE_T, RHI::Sampler*> samplerCache{};

		HashMap<BuiltinDrawItemTag, RHI::DrawListTag> builtinDrawTags{};

		b8 isFirstTick = true;
		clock_t startTime;
		f32 currentTime = 0.0f;

		friend class RPI::Scene;
	};
    
} // namespace CE::RPI
