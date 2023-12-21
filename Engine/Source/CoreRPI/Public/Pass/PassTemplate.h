#pragma once

namespace CE::RPI
{
	struct PassRequest
	{
		/// @brief Name of the pass after instantiation
		Name passName{};

		/// @brief Name of pass template we will instantiate from
		Name templateName{};

		/// @brief Connections of the instantiated pass
		Array<PassConnection> connections{};

		Array<PassImageAttachmentDesc> imageAttachmentOverrides{};

	};
    
	class CORERPI_API PassTemplate final : public IntrusiveBase
	{
        friend class Pass;
	public:
        
        
	private:
        
        Name name{};
        
		Name passClass{};
        
        Array<PassSlot> slots{};

		Array<PassConnection> connections{};

		Array<PassImageAttachmentDesc> imageAttachments{};

		Array<PassRequest> passRequest{};

	};

} // namespace CE::RPI
