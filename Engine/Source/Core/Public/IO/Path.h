#pragma once

#include "Containers/String.h"

#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace CE::IO
{
    class FileStream;
    using RecursiveDirectoryIterator = fs::recursive_directory_iterator;
    
    class CORE_API Path
    {
    public:
        Path();
        ~Path();

        inline Path(fs::path path) : impl(path)
        {}

        inline Path(String pathString) : impl(pathString.GetCString())
        {}

        inline Path(std::string pathString) : impl(pathString)
        {}

        inline Path(const char* cString) : impl(cString)
        {}

        template<typename It>
        inline Path(It first, It last) : impl(first, last)
        {}

        inline String GetString() const
        {
            return impl.string();
        }

        inline bool IsRelative() const
        {
            return impl.is_relative();
        }

        inline bool IsAbsolute() const
        {
            return impl.is_absolute();
        }

        inline friend Path operator/(const Path& lhs, const Path& rhs)
        {
            return Path(lhs.impl / rhs.impl);
        }

        inline friend Path operator/(const Path& lhs, const String& rhs)
        {
            return Path(lhs.impl / rhs.GetCString());
        }

        inline friend Path operator/(const Path& lhs, const char* rhs)
        {
            return Path(lhs.impl / rhs);
        }

        inline friend bool operator==(const Path& lhs, const Path& rhs)
        {
            return lhs.impl == rhs.impl;
        }

        inline friend bool operator!=(const Path& lhs, const Path& rhs)
        {
            return lhs.impl != rhs.impl;
        }

        inline operator fs::path() const
        {
            return impl;
        }

        inline bool Exists() const
        {
            return fs::exists(impl);
        }

        inline bool IsEmpty() const
        {
            return impl.empty();
        }

        inline bool IsDirectory() const
        {
            return fs::is_directory(impl);
        }

        inline Path GetParentPath() const
        {
            return Path(impl.parent_path());
        }

        inline Path GetFilename() const
        {
            return impl.filename();
        }

        inline Path GetExtension() const
        {
            return impl.extension();
        }

        inline Path RemoveExtension() const
        {
            fs::path p = impl;
            return p.replace_extension();
        }

        inline Path ReplaceExtension(const Path& path) const
        {
            fs::path p = impl;
            return p.replace_extension(path.impl);
        }

        static inline Path GetRelative(const Path& path, const Path& base)
        {
            return fs::relative(path.impl, base.impl);
        }

        static inline bool IsSubDirectory(Path path, Path root)
        {
            while (path != Path())
            {
                if (path == root)
                {
                    return true;
                }
                path = path.GetParentPath();
            }
            return false;
        }

        friend inline std::ostream& operator<<(std::ostream& os, const Path& path)
        {
            os << path.impl;
            return os;
        }

        inline static bool CreateDirectories(IO::Path path)
        {
            return fs::create_directories(path.impl);
        }

        inline static void Copy(IO::Path from, IO::Path to)
        {
            fs::copy(from.impl, to.impl, fs::copy_options::recursive);
        }
        
        inline static bool Remove(IO::Path path)
        {
            return fs::remove(path.impl);
        }
        
        inline static u32 RemoveRecursively(IO::Path path)
        {
            return fs::remove_all(path.impl);
        }

        inline static void Rename(const IO::Path& from, const IO::Path& to)
        {
            fs::rename(from.impl, to.impl);
        }

        void IterateChildren(std::function<void(const IO::Path& path)> func);
        void RecursivelyIterateChildren(std::function<void(const IO::Path& path)> func);

        auto begin() { return impl.begin(); }
        auto end() { return impl.end(); }

        const auto begin() const { return impl.begin(); }
        const auto end() const { return impl.end(); }

    private:
        fs::path impl;

        friend class CE::IO::FileStream;

        template<typename T>
        friend SIZE_T CE::GetHash(const T& value);
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

namespace CE
{
    template<>
    inline SIZE_T GetHash<CE::IO::Path>(const CE::IO::Path& value)
    {
        return fs::hash_value(value.impl);
    }

}
