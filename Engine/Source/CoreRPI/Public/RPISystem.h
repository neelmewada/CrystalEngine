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

	private:

		RPISystem() = default;

		RHI::RHISystem rhiSystem{};

		Array<ScenePtr> scenes{};

	};
    
} // namespace CE::RPI
