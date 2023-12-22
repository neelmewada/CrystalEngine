#pragma once

namespace CE::RPI
{

	class CORERPI_API PassSystem
	{
	public:

		void Initialize();

		void Shutdown();

	private:

		/// @brief Used to store pass templates.
		PassLibrary passLibrary{};

		/// @brief Used to store pass factories.
		PassFactory passFactory{};

	};
    
} // namespace CE::RPI
