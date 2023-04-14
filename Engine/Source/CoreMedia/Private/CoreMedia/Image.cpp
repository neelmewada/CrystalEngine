
#include "CoreMedia/Image.h"

#include "stb_image.h"

namespace CE
{

    CMImage::CMImage()
    {

    }

    CMImage::~CMImage()
    {

    }

    CMImageInfo CMImage::GetImageInfoFromFile(IO::Path filePath)
    {
        CMImageInfo info{};
        if (!filePath.Exists() || filePath.IsDirectory())
        {
            info.failureReason = "File doesn't exist";
            return info;
        }

        auto filePathStr = filePath.GetString();
        auto result = stbi_info(filePathStr.GetCString(), &info.x, &info.y, &info.numChannels);
        if (result != 1)
        {
            info.failureReason = stbi_failure_reason();
        }

        return info;
    }

    CMImageInfo CMImage::GetImageInfoFromMemory(unsigned char* buffer, int bufferLength)
    {
        CMImageInfo info{};

        auto result = stbi_info_from_memory(buffer, bufferLength, &info.x, &info.y, &info.numChannels);
        if (result != 1)
        {
            info.failureReason = stbi_failure_reason();
        }

        return info;
    }

    CMImage CMImage::LoadFromFile(IO::Path filePath, int desiredNumChannels)
    {
        CMImage image{};

        if (!filePath.Exists() || filePath.IsDirectory())
        {
            image.failureReason = "File doesn't exist";
            return image;
        }

        auto filePathStr = filePath.GetString();
        image.data = stbi_load(filePathStr.GetCString(), &image.x, &image.y, &image.numChannels, desiredNumChannels);
        if (!image.IsValid())
            image.failureReason = stbi_failure_reason();
        return image;
    }

    CMImage CMImage::LoadFromMemory(unsigned char* buffer, int bufferLength, int desiredNumChannels)
    {
        CMImage image{};
        image.data = stbi_load_from_memory(buffer, bufferLength, &image.x, &image.y, &image.numChannels, desiredNumChannels);
        if (!image.IsValid())
            image.failureReason = stbi_failure_reason();
        return image;
    }

    void CMImage::Free()
    {
        if (!IsValid())
            return;

        stbi_image_free(data);

        data = nullptr;
        x = y = numChannels = 0;
        failureReason = nullptr;
    }

}
