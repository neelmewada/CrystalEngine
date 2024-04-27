#include "CoreRPI.h"

namespace CE::RPI
{
    PassAttachment::PassAttachment(const PassImageAttachmentDesc& desc)
    {
        name = desc.name;
        lifetime = desc.lifetime;
        sizeSource = desc.sizeSource;
        fallbackFormats = desc.fallbackFormats;
        
        attachmentDescriptor = RPI::UnifiedAttachmentDescriptor(desc.imageDescriptor);
    }

    PassAttachment::PassAttachment(const PassBufferAttachmentDesc& desc)
    {
        name = desc.name;
        lifetime = desc.lifetime;
        sizeSource = desc.sizeSource;

        attachmentDescriptor = RPI::UnifiedAttachmentDescriptor(desc.bufferDescriptor);
    }
    
    Ptr<PassAttachment> PassAttachmentBinding::GetActualAttachment() const
    {
        if (attachment)
            return attachment;
        if (connectedBinding)
            return connectedBinding->GetActualAttachment();
        
        return nullptr;
    }

} // namespace CE::RPI
