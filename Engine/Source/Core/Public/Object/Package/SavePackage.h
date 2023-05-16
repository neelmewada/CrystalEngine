#pragma once

namespace CE
{
    enum class LoadPackageResult
    {
        Success = 0,
        PackageNotFound,
        InvalidPackage,
    };

    enum class SavePackageResult
    {
        Success = 0,
        UnknownError,
        InvalidPath
    };

    struct SavePackageArgs
    {
        
    };

} // namespace CE
