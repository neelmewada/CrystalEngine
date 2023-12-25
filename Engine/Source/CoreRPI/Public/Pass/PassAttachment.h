#pragma once

namespace CE::RPI
{
	/// @brief Describes the attachment used by a pass
	struct CORERPI_API PassAttachment final : public IntrusiveBase
	{
	public:

		PassAttachment(const PassImageAttachmentDesc& desc);
		PassAttachment(const PassBufferAttachmentDesc& desc);

		/// @brief Name of the attachment.
		Name name{};

		RHI::AttachmentLifetimeType lifetime = RHI::AttachmentLifetimeType::Transient;

		RHI::UnifiedScopeAttachmentDesc unifiedAttachmentDesc{};

	};

	/// @brief A Pass attachment binding either points to a pass attachment OR to another
	/// pass attachment binding, for example, an input slot of one pass connected to output
	/// of another.
	struct CORERPI_API PassAttachmentBinding final
	{
	public:

		/// @brief Name of the attachment binding. Either pass slot name, or attachment name.
		Name name = "";

		/// @brief Name of shader input field.
		Name shaderInputName = "";

		PassSlotType slotType = PassSlotType::Undefined;

		RHI::ScopeAttachmentUsage attachmentUsage = RHI::ScopeAttachmentUsage::None;

		/// @brief Pointer to another attachment binding slot this binding is connected to. For InputOutput slots,
		/// this is always an input connection.
		PassAttachmentBinding* connectedBinding = nullptr;

	private:

		Ptr<PassAttachment> attachment = nullptr;

	};

} // namespace CE::RPI

