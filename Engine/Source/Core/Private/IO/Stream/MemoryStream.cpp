/*
* Copyright(c) Contributors to the Open 3D Engine Project.
*
*SPDX - License - Identifier: Apache - 2.0 OR MIT
*
*/

#include "IO/Stream/MemoryStream.h"

namespace CE::IO
{

	void MemoryStream::Seek(SSIZE_T bytes, SeekMode mode)
	{
		currentOffset = static_cast<SIZE_T>(ComputeSeekPosition(bytes, mode));
	}

	SIZE_T MemoryStream::Read(SIZE_T bytes, void* outBuffer)
	{
        if (currentOffset >= GetLength())
        {
            return 0;
        }
        SIZE_T bytesLeft = GetLength() - currentOffset;
        if (bytes > bytesLeft)
        {
            bytes = bytesLeft;
        }
        if (bytes)
        {
            memcpy(outBuffer, buffer + currentOffset, static_cast<SIZE_T>(bytes));
            currentOffset += static_cast<SIZE_T>(bytes);
        }
        return bytes;
	}

    SIZE_T MemoryStream::PrepareForWrite(SIZE_T bytes)
    {
        if (currentOffset >= bufferLength)
        {
            return 0;
        }
        SIZE_T bytesLeft = bufferLength - currentOffset;
        if (bytes > bytesLeft)
        {
            bytes = bytesLeft;
        }
        return bytes;
    }

	SIZE_T MemoryStream::Write(SIZE_T bytes, const void* inBuffer)
	{
        bytes = PrepareForWrite(bytes);
        if (bytes)
        {
            memcpy(const_cast<char*>(buffer) + currentOffset, inBuffer, static_cast<size_t>(bytes));
            currentOffset += static_cast<size_t>(bytes);
            currentLength = CE::Math::Max(currentOffset, currentLength);
        }
        return bytes;
	}

} // namespace CE::IO
