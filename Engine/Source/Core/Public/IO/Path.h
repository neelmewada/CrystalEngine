#pragma once

#include "Containers/String.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace CE::IO
{
    class FileStream;
    
    class CORE_API Path
    {
    public:
        Path();
        ~Path();

        inline Path(fs::path path) : Impl(path)
        {}

        inline Path(String pathString) : Impl(pathString.GetCString())
        {}

        inline Path(std::string pathString) : Impl(pathString)
        {}

        inline Path(const char* cString) : Impl(cString)
        {}

        inline String GetString() const
        {
            return Impl.string();
        }

        inline bool IsRelative() const
        {
            return Impl.is_relative();
        }

        inline bool IsAbsolute() const
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

        inline friend bool operator==(const Path& lhs, const Path& rhs)
        {
            return lhs.Impl == rhs.Impl;
        }

        inline friend bool operator!=(const Path& lhs, const Path& rhs)
        {
            return lhs.Impl != rhs.Impl;
        }

        inline bool Exists() const
        {
            return fs::exists(Impl);
        }

        inline bool IsEmpty() const
        {
            return Impl.empty();
        }

        inline bool IsDirectory() const
        {
            return fs::is_directory(Impl);
        }

        inline Path GetParentPath() const
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

        inline static bool CreateDirectories(IO::Path path)
        {
            return fs::create_directories(path.Impl);
        }

    private:
        fs::path Impl;

        friend class CE::IO::FileStream;
    };

} // namespace CE::IO

/// fmt user-defined Formatter for CE::IO::Path
template <> struct fmt::formatter<CE::IO::Path> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::IO::Path& path, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", path.GetString());
    }
};

