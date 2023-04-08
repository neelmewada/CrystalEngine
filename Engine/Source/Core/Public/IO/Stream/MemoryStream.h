/*
* Copyright(c) Contributors to the Open 3D Engine Project.
*
* SPDX - License - Identifier: Apache - 2.0 OR MIT
*
*/

#pragma once

#include "GenericStream.h"

namespace CE::IO
{

    class CORE_API MemoryStream : public GenericStream
    {
    public:
        enum MemoryStreamMode
        {
            MSM_READONLY,
            MSM_READWRITE,
        };

        /// Initializes as an invalid memory stream
        MemoryStream()
            : buffer(nullptr)
            , bufferLength(0)
            , currentLength(0)
            , mode(mode)
            , isAllocated(false)
        {}

        MemoryStream(SIZE_T bufferLength)
            : buffer(new char[bufferLength])
            , bufferLength(bufferLength)
            , currentLength(bufferLength)
            , mode(MSM_READWRITE)
            , isAllocated(true)
        {
            memset((void*)buffer, 0, bufferLength);
        }

        MemoryStream(const char* buffer, SIZE_T bufferLength, MemoryStreamMode mode = MSM_READONLY)
            : buffer(buffer)
            , bufferLength(bufferLength)
            , currentLength(bufferLength)
            , mode(mode)
            , isAllocated(false)
        {}

        /// Creates a memory stream from given buffer pointer. 'currentLength' refers to the size of meaningful data actually stored, whereas 'bufferLength' refers to size of the buffer
        MemoryStream(const char* buffer, SIZE_T bufferLength, SIZE_T currentLength, MemoryStreamMode mode = MSM_READWRITE)
            : buffer(buffer)
            , bufferLength(bufferLength)
            , currentLength(currentLength)
            , mode(mode)
            , isAllocated(false)
        {}

        virtual ~MemoryStream()
        {
            
        }

        bool        IsOpen() const override { return buffer != NULL; }
        bool        CanSeek() const override { return true; }
        bool        CanRead() const override { return true; }
        bool        CanWrite() const override { return mode == MSM_READWRITE; }
        void        Seek(SSIZE_T bytes, SeekMode mode) override;
        SIZE_T      Read(SIZE_T bytes, void* outBuffer) override;
        u8          ReadNextByte() override;
        SIZE_T      Write(SIZE_T bytes, const void* inBuffer) override;
        
        virtual const char* GetRawPointer() const override { return buffer; }
        
        virtual const void* GetData() const { return buffer; }
        SIZE_T      GetCurPos() const override { return currentOffset; }
        SIZE_T      GetLength() const override { return currentLength; }

        CE_INLINE bool IsValid() const
        {
            return buffer != nullptr;
        }

        CE_INLINE bool IsAllocated() const
        {
            return isAllocated;
        }

        CE_INLINE void Free()
        {
            if (isAllocated)
            {
                isAllocated = false;
                delete buffer;
                buffer = nullptr;
            }
        }

        CE_INLINE const char* GetBuffer() const
        {
            return buffer;
        }

    protected:
        SIZE_T      PrepareForWrite(SIZE_T bytes);

        bool isAllocated = false;
        const char* buffer = nullptr;
        SIZE_T bufferLength = 0;
        SIZE_T currentLength = 0;
        SIZE_T currentOffset = 0;
        MemoryStreamMode mode = MSM_READONLY;
    };
    
} // namespace CE::IO
