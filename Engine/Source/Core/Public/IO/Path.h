#pragma once

#include "Containers/String.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace CE::IO
{
    
    class CORE_API Path
    {
    public:
        Path();
        ~Path();

        inline Path(fs::path path) : Impl(path)
        {}

        inline Path(String pathString) : Impl(pathString.GetCString())
        {

        }

        inline Path(std::string pathString) : Impl(pathString)
        {
            
        }

        inline String GetString()
        {
            return Impl.string();
        }

        inline bool IsRelative()
        {
            return Impl.is_relative();
        }

        inline bool IsAbsolute()
        {
            return Impl.is_absolute();
        }

        inline friend Path operator/(const Path& lhs, const Path& rhs)
        {
            return Path(lhs.Impl / rhs.Impl);
        }

        inline friend Path operator/(const Path& lhs, const String& rhs)
        {
            return Path(lhs.Impl / rhs.GetCString());
        }

        inline friend Path operator/(const Path& lhs, const char* rhs)
        {
            return Path(lhs.Impl / rhs);
        }

        inline bool Exists()
        {
            return fs::exists(Impl);
        }

        inline bool IsEmpty()
        {
            return Impl.empty();
        }

        inline bool IsDirectory()
        {
            return fs::is_directory(Impl);
        }

        inline Path GetParentPath()
        {
            return Path(Impl.parent_path());
        }

        static inline Path GetRelative(const Path& path, const Path& base)
        {
            return fs::relative(path.Impl, base.Impl);
        }

        friend inline std::ostream& operator<<(std::ostream& os, const Path& path)
        {
            os << path.Impl;
            return os;
        }

    private:
        fs::path Impl;
    };

} // namespace CE::IO

