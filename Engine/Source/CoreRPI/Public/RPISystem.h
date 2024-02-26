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

	private:

		RPISystem() = default;

		void CreateDefaultTextures();

		RHI::RHISystem rhiSystem{};

		Array<ScenePtr> scenes{};

		RPI::Texture* defaultAlbedoTex = nullptr;
		RPI::Texture* defaultNormalTex = nullptr;
		RPI::Texture* defaultRoughnessTex = nullptr;

		SharedMutex samplerCacheMutex{};
		HashMap<SIZE_T, RHI::Sampler*> samplerCache{};
	};
    
} // namespace CE::RPI
