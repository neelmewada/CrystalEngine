#pragma once

namespace CE::RPI
{
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

		inline RHI::Texture* GetDefaultAlbedoTex() const
		{
			return defaultAlbedoTex;
		}

		inline RHI::Texture* GetDefaultRoughnessTex() const
		{
			return defaultRoughnessTex;
		}

		inline RHI::Texture* GetDefaultNormalTex() const
		{
			return defaultNormalTex;
		}

		RHI::Sampler* FindOrCreateSampler(const RHI::SamplerDescriptor& desc);

	private:

		RPISystem() = default;

		void CreateDefaultTextures();

		RHI::RHISystem rhiSystem{};

		Array<ScenePtr> scenes{};

		RHI::Texture* defaultAlbedoTex = nullptr;
		RHI::Texture* defaultNormalTex = nullptr;
		RHI::Texture* defaultRoughnessTex = nullptr;

		SharedMutex samplerCacheMutex{};
		HashMap<SIZE_T, RHI::Sampler*> samplerCache{};
	};
    
} // namespace CE::RPI
