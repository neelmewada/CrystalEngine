/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#include "IO/Stream/GenericStream.h"

namespace CE::IO
{

    SIZE_T GenericStream::ReadAtOffset(SIZE_T bytes, void* outBuffer, SSIZE_T offsetInBytes)
    {
        if (offsetInBytes >= 0)
        {
            Seek(offsetInBytes, SeekMode::ST_SEEK_BEGIN);
        }
        return Read(bytes, outBuffer);
    }

    SIZE_T GenericStream::WriteAtOffset(SIZE_T bytes, const void* inBuffer, SSIZE_T offsetInBytes)
    {
        if (offsetInBytes >= 0)
        {
            Seek(offsetInBytes, SeekMode::ST_SEEK_BEGIN);
        }
        return Write(bytes, inBuffer);
    }

    SIZE_T GenericStream::ComputeSeekPosition(SSIZE_T offsetInBytes, SeekMode mode)
    {
        SIZE_T absBytes = static_cast<SIZE_T>(abs(offsetInBytes));
        if (mode == ST_SEEK_BEGIN)
        {
            if (offsetInBytes > 0)
            {
                return absBytes;
            }
            else
            {
                return 0;
            }
        }
        else if (mode == ST_SEEK_CUR)
        {
            SIZE_T curPos = GetCurPos();
            if (offsetInBytes > 0)
            {
                return curPos + absBytes;
            }
            else
            {
                if (curPos >= absBytes)
                {
                    return curPos - absBytes;
                }
                else
                {
                    return 0;
                }
            }
        }
        else // mode == ST_SEEK_END
        {
            SIZE_T curLen = GetLength();
            if (offsetInBytes > 0)
            {
                return curLen + absBytes;
            }
            else
            {
                if (curLen >= absBytes)
                {
                    return curLen - absBytes;
                }
                else
                {
                    return 0;
                }
            }
        }
    }

} // namespace CE::IO
