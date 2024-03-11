#pragma once

namespace CE::RPI
{
	class Texture;

	/// @brief RPISystem owns all scenes.
	class CORERPI_API RPISystem final
	{
		CE_NO_COPY(RPISystem)
	public:

		static RPISystem& Get();

		inline RHI::DrawListTagRegistry* GetDrawListTagRegistry()
		{
			return rhiSystem.GetDrawListTagRegistry();
		}

		void Initialize();

		void Shutdown();

		inline RPI::Texture* GetDefaultAlbedoTex() const
		{
			return defaultAlbedoTex;
		}

		inline RPI::Texture* GetDefaultRoughnessTex() const
		{
			return defaultRoughnessTex;
		}

		inline RPI::Texture* GetDefaultNormalTex() const
		{
			return defaultNormalTex;
		}

		RHI::Sampler* FindOrCreateSampler(const RHI::SamplerDescriptor& desc);

		inline const Array<RHI::VertexBufferView>& GetFullScreenQuad() const { return quadVertexBufferViews; }
		inline RHI::DrawLinearArguments GetFullScreenQuadDrawArgs() const { return quadDrawArgs; }

		inline const Array<RHI::VertexBufferView>& GetTextQuad() const { return textQuadVertexBufferViews; }
		inline RHI::DrawLinearArguments GetTextQuadDrawArgs() const { return textQuadDrawArgs; }

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

		Array<ScenePtr> scenes{};

		RPI::Texture* defaultAlbedoTex = nullptr;
		RPI::Texture* defaultNormalTex = nullptr;
		RPI::Texture* defaultRoughnessTex = nullptr;

		SharedMutex samplerCacheMutex{};
		HashMap<SIZE_T, RHI::Sampler*> samplerCache{};
	};
    
} // namespace CE::RPI
