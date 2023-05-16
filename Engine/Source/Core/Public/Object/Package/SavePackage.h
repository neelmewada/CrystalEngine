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
    };

    struct SavePackageArgs
    {
        
    };
    
    class CORE_API SavePackageContext
    {
    public:
        
    };

} // namespace CE
