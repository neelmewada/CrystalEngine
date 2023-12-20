#pragma once

namespace CE::RPI
{
    
	class CORERPI_API PassTemplate final : public IntrusiveBase
	{
        friend class Pass;
	public:
        
        
	private:
        
        Name name{};
        
		Name passClass{};
        
        Array<PassSlot> slots{};

	};

} // namespace CE::RPI
