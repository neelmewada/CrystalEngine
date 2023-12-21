#pragma once

namespace CE::RPI
{

	class CORERPI_API RPISystem final
	{
	public:

		void Initialize();

		void Shutdown();

	private:

		Array<ScenePtr> scenes{};
        
		PassSystem passSystem{};

	};
    
} // namespace CE::RPI
