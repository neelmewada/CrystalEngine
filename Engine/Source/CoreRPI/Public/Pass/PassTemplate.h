#pragma once

namespace CE::RPI
{
	/// @brief Describes the pass to create as a subpass of current pass.
	struct PassRequest
	{
		/// @brief Name of the pass after instantiation
		Name passName{};

		/// @brief Name of pass template we will instantiate from
		Name templateName{};

		/// @brief Connections of the instantiated pass
		Array<PassConnection> connections{};

		/// @brief Override for image attachments.
		Array<PassImageAttachmentDesc> imageAttachmentOverrides{};

	};
    
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
