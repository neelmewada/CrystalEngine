#pragma once

namespace CE::RPI
{

	class CORERPI_API Scene final : public IntrusiveBase
	{
	public:

		Scene();
		~Scene();

	private:

		Array<RenderPipeline*> renderPipelines{};

	};

	typedef Ptr<Scene> ScenePtr;
    
} // namespace CE::RPI
