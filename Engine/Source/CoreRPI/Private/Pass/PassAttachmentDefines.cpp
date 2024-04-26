#include "CoreRPI.h"

namespace CE::RPI
{

    UnifiedAttachmentDescriptor::UnifiedAttachmentDescriptor(const UnifiedAttachmentDescriptor& copy)
    {
        if (copy.type == AttachmentType::Image)
        {
            if (copy.type == type)
            {
                imageDesc = copy.imageDesc;
            }
            else // Copy is a buffer
            {
                if (type == AttachmentType::Image)
                    imageDesc.~ImageDescriptor();
                memset(&bufferDesc, 0, sizeof(bufferDesc));
                bufferDesc = copy.bufferDesc;
            }
            type = copy.type;
        }
        else if (copy.type == AttachmentType::Buffer)
        {
            if (copy.type == type)
            {
                bufferDesc = copy.bufferDesc;
            }
            else // Copy is an image
            {
                if (type == AttachmentType::Buffer)
                    bufferDesc.~BufferDescriptor();
                memset(&imageDesc, 0, sizeof(imageDesc));
                imageDesc = copy.imageDesc;
            }
            type = copy.type;
        }
    }
    
    UnifiedAttachmentDescriptor& UnifiedAttachmentDescriptor::operator=(const UnifiedAttachmentDescriptor& copy)
    {
        if (copy.type == AttachmentType::Image)
        {
            if (copy.type == type)
            {
                imageDesc = copy.imageDesc;
            }
            else // Copy is a buffer
            {
                if (type == AttachmentType::Image)
                    imageDesc.~ImageDescriptor();
                memset(&bufferDesc, 0, sizeof(bufferDesc));
                bufferDesc = copy.bufferDesc;
            }
            type = copy.type;
        }
        else if (copy.type == AttachmentType::Buffer)
        {
            if (copy.type == type)
            {
                bufferDesc = copy.bufferDesc;
            }
            else // Copy is an image
            {
                if (type == AttachmentType::Buffer)
                    bufferDesc.~BufferDescriptor();
                memset(&imageDesc, 0, sizeof(imageDesc));
                imageDesc = copy.imageDesc;
            }
            type = copy.type;
        }
        return *this;
    }

    UnifiedAttachmentDescriptor::~UnifiedAttachmentDescriptor()
    {
        if (type == AttachmentType::Image)
        {
            imageDesc.~ImageDescriptor();
        }
        else if (type == AttachmentType::Buffer)
        {
            bufferDesc.~BufferDescriptor();
        }

        type = AttachmentType::None;
    }

} // namespace CE::RPI
