#pragma once

namespace CE::RPI
{
    
	class CORERPI_API PassTree final
	{
	public:

		virtual ~PassTree();

	private:

		ParentPass* rootPass = nullptr;

	};

} // namespace CE::RPI
