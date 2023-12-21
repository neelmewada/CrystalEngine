#pragma once

namespace CE::RPI
{
    
	class CORERPI_API PassFactory final
	{
	public:


	private:

		HashMap<Name, PassCreatorFunc> passCreatorsByClassName{};

	};

} // namespace CE::RPI
