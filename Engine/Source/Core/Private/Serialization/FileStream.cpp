
#include "CoreMinimal.h"

namespace CE
{

    /*FileAsciiStream::FileAsciiStream(const IO::Path& filePath, Permissions openMode)
    {
        ASSERT(openMode != Permissions::None, "FileAsciiStream constructed with openMode as None!")

        this->filePath = filePath;

        this->openMode = openMode;
        std::ios::openmode mode = std::ios::binary;
        if (openMode == Permissions::ReadOnly)
        {
            mode |= std::ios::in;
        }
        else if (openMode == Permissions::WriteOnly)
        {
            mode |= std::ios::out;
        }
        else if (openMode == Permissions::ReadWrite)
        {
            mode |= std::ios::in | std::ios::out;
        }

        if ((!filePath.Exists() && (mode & std::ios::in) != 0) || filePath.IsDirectory())
        {
            CE_LOG(Error, All, "FileStream constructed with a path that either does not exist or is a directory: {}", filePath);
            return;
        }

        impl = std::fstream((fs::path)filePath, mode);
    }

    FileAsciiStream::~FileAsciiStream()
    {
        if (impl.is_open())
            impl.close();
    }*/
    
} // namespace CE

