#pragma once

namespace CE::RPI
{

	class CORERPI_API Scene final
	{
	public:

		Scene();
		~Scene();

	private:

		Array<RenderPipelinePtr> renderPipelines{};

	};

	typedef Scene* ScenePtr;
    
} // namespace CE::RPI
