#include "CoreRPI.h"

namespace CE::RPI
{

    UnifiedAttachmentDescriptor::UnifiedAttachmentDescriptor(const UnifiedAttachmentDescriptor& copy)
    {
        if (copy.type == AttachmentType::Image)
        {
            if (copy.type == type || type == AttachmentType::None) // Both are image types
            {
                imageDesc = copy.imageDesc;
            }
            else if (copy.type == AttachmentType::Buffer) // Copy is a buffer
            {
                if (type == AttachmentType::Image)
                    imageDesc.~ImageAttachmentDescriptor();
                memset(&bufferDesc, 0, sizeof(bufferDesc));
                new(&bufferDesc) RPI::BufferAttachmentDescriptor(copy.bufferDesc);
            }
            type = copy.type;
        }
        else if (copy.type == AttachmentType::Buffer)
        {
            if (copy.type == type || type == AttachmentType::None) // Both are buffer types
            {
                bufferDesc = RPI::BufferAttachmentDescriptor(copy.bufferDesc);
            }
            else if (copy.type == AttachmentType::Image) // Copy is an image
            {
                if (type == AttachmentType::Buffer)
                    bufferDesc.~BufferAttachmentDescriptor();
                memset(&imageDesc, 0, sizeof(imageDesc));
                new(&imageDesc) RPI::ImageAttachmentDescriptor(copy.imageDesc);
            }
            type = copy.type;
        }
    }
    
    UnifiedAttachmentDescriptor& UnifiedAttachmentDescriptor::operator=(const UnifiedAttachmentDescriptor& copy)
    {
        if (copy.type == AttachmentType::Image)
        {
            if (copy.type == type || type == AttachmentType::None)
            {
                imageDesc = RPI::ImageAttachmentDescriptor(copy.imageDesc);
            }
            else if (copy.type == AttachmentType::Buffer) // Copy is a buffer
            {
                if (type == AttachmentType::Image)
                    imageDesc.~ImageAttachmentDescriptor();
                memset(&bufferDesc, 0, sizeof(bufferDesc));
                new(&bufferDesc) RPI::BufferAttachmentDescriptor(copy.bufferDesc);
            }
            type = copy.type;
        }
        else if (copy.type == AttachmentType::Buffer)
        {
            if (copy.type == type || type == AttachmentType::None)
            {
                bufferDesc = RPI::BufferAttachmentDescriptor(copy.bufferDesc);
            }
            else if (copy.type == AttachmentType::Image) // Copy is an image
            {
                if (type == AttachmentType::Buffer)
                    bufferDesc.~BufferAttachmentDescriptor();
                memset(&imageDesc, 0, sizeof(imageDesc));
                new(&imageDesc) RPI::ImageAttachmentDescriptor(copy.imageDesc);
            }
            type = copy.type;
        }
        return *this;
    }

    UnifiedAttachmentDescriptor::~UnifiedAttachmentDescriptor()
    {
        memset(this, 0, sizeof(UnifiedAttachmentDescriptor));
    }

} // namespace CE::RPI
