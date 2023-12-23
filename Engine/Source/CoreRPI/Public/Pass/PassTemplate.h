#pragma once

namespace CE::RPI
{
    
	class CORERPI_API PassTemplate final : public IntrusiveBase
	{
        friend class Pass;
	public:
        
		static Ptr<PassTemplate> CreateFromJson(const String& json);
        
	private:
        
        Name name{};
        
		Name passClass{};
        
        Array<PassSlot> slots{};

		Array<PassConnection> connections{};

		Array<PassImageAttachmentDesc> imageAttachments{};

		Array<PassRequest> childPasses{};

	};

} // namespace CE::RPI
