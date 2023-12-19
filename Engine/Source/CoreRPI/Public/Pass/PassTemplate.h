#pragma once

namespace CE::RPI
{
    
	class CORERPI_API PassTemplate final
	{
	public:
        
        
	private:
        
        Name name{};
        
        SubClassType<Pass> passClass{};
        
        Array<PassSlot> slots{};
	};

} // namespace CE::RPI
