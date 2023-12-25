#include "CoreRPI.h"

namespace CE::RPI
{
    PassAttachment::PassAttachment(const PassImageAttachmentDesc& desc)
    {
        name = desc.name;
        lifetime = desc.lifetime;
        
        attachmentDescriptor = RPI::UnifiedAttachmentDescriptor(desc.imageDescriptor);
    }

    PassAttachment::PassAttachment(const PassBufferAttachmentDesc& desc)
    {
        name = desc.name;
        lifetime = desc.lifetime;
        
        attachmentDescriptor = RPI::UnifiedAttachmentDescriptor(desc.bufferDescriptor);
    }

} // namespace CE::RPI
