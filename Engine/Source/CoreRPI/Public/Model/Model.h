#pragma once

namespace CE::RPI
{
    
	class CORERPI_API Model final
	{
	public:

		Model();
		~Model();

	private:

		Array<ModelLod*> lods{};
	};

} // namespace CE::RPI
