#pragma once

namespace CE::RPI
{

	class CORERPI_API PassSystem
	{
	public:

		void Initialize();

		void Shutdown();

	private:

		PassLibrary passLibrary{};

		PassFactory passFactory{};

	};
    
} // namespace CE::RPI
