#include "CoreRPI.h"

namespace CE::RPI
{
    PassAttachment::PassAttachment(const PassImageAttachmentDesc& desc)
    {
        name = desc.name;
        lifetime = desc.lifetime;
        sizeSource = desc.sizeSource;
        fallbackFormats = desc.fallbackFormats;
        formatSource = desc.formatSource;
        
        attachmentDescriptor = RPI::UnifiedAttachmentDescriptor(desc.imageDescriptor);
    }

    PassAttachment::PassAttachment(const PassBufferAttachmentDesc& desc)
    {
        name = desc.name;
        lifetime = desc.lifetime;
        sizeSource = desc.sizeSource;

        attachmentDescriptor = RPI::UnifiedAttachmentDescriptor(desc.bufferDescriptor);
    }
    
    Ptr<PassAttachment> PassAttachmentBinding::GetOriginalAttachment() const
    {
        if (attachment)
            return attachment;
        if (originalAttachment)
            return originalAttachment;
        if (connectedBinding)
            return originalAttachment = connectedBinding->GetOriginalAttachment();
        
        return nullptr;
    }

} // namespace CE::RPI
