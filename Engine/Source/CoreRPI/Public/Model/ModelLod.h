#pragma once

namespace CE::RPI
{
	struct Mesh
	{

	};

	class CORERPI_API ModelLod final
	{
	public:

		ModelLod();
		virtual ~ModelLod();


	private:

		Array<Mesh> meshes{};

	};
    
} // namespace CE::RPI

