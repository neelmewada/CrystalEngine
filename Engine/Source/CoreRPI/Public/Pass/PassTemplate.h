#pragma once

namespace CE::RPI
{
    
	class CORERPI_API PassTemplate final
	{
	public:
        
        
	private:
        
        Name name{};
        
		Name passClass{};
        
        Array<PassSlot> slots{};

		/// @brief Connections are defined only for the input slots.
		/// Because 1 output can be connected to multiple inputs but not vice versa.
		Array<PassConnection> connections{};

	};

} // namespace CE::RPI
