/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */
#pragma once

#include "IO/OpenMode.h"

namespace CE::IO
{
    
    /// Base class for all kinds of streams
    class CORE_API GenericStream
    {
    public:
        enum SeekMode
        {
            ST_SEEK_BEGIN,
            ST_SEEK_CUR,
            ST_SEEK_END,
        };

        virtual ~GenericStream() {}

        virtual bool        IsOpen() const = 0;
        virtual bool        CanSeek() const = 0;
        virtual bool        CanRead() const = 0;
        virtual bool        CanWrite() const = 0;
        virtual void        Seek(SSIZE_T offsetInBytes, SeekMode mode) = 0;
        virtual SIZE_T      Read(SIZE_T bytes, void* outBuffer) = 0;
        virtual SIZE_T      Write(SIZE_T bytes, const void* inBuffer) = 0;

        virtual SIZE_T      GetCurPos() const = 0;
        virtual SIZE_T      GetLength() const = 0;
        virtual SIZE_T      ReadAtOffset(SIZE_T bytes, void* oBuffer, SSIZE_T offsetInBytes = -1);
        virtual SIZE_T      WriteAtOffset(SIZE_T bytes, const void* iBuffer, SSIZE_T offsetInBytes = -1);
        virtual bool        IsCompressed() const { return false; }
        virtual const char* GetFilename() const { return ""; }
        virtual OpenMode    GetModeFlags() const { return OpenMode(); }
        virtual bool        ReOpen() { return true; }
        virtual void        Close() {}

    protected:
        SIZE_T ComputeSeekPosition(SSIZE_T offsetInBytes, SeekMode mode);
    };

} // namespace CE::IO
