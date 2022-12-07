#pragma once

#include "Algorithm/Hash.h"
#include "Containers/String.h"
#include "Containers/Array.h"

namespace CE
{

    /// A type used to define path to Reflected c++ types & members
    /// Ex: ParentNamespace::ChildNamespace::SomeClass
    class CORE_API Name
    {
    public:
        Name(String name);
        Name(const char* name);
        ~Name() = default;

        inline SIZE_T GetHashValue() const { return HashValue; }

        inline String GetString() const { return Value; }

        inline bool operator==(const Name& rhs) const
        {
            return HashValue == rhs.HashValue;
        }

        inline bool operator!=(const Name& rhs) const
        {
            return HashValue != rhs.HashValue;
        }

        inline u32 GetComponentCount()
        {
            return Components.GetSize();
        }

        inline StringView GetComponentAt(u32 index)
        {
            return StringView(Components[index]);
        }

    private:
        String Value;
        SIZE_T HashValue;

        CE::Array<String> Components{};
    };

    template<>
    inline SIZE_T GetHash(const Name& Value)
    {
        return Value.GetHashValue();
    }
    
} // namespace CE


/// fmt user-defined Formatter for CE::String
template <> struct fmt::formatter<CE::Name> {
    // Parses format specifications of the form ['f' | 'e'].
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        // Return an iterator past the end of the parsed range:
        return ctx.end();
    }

    // Formats the point p using the parsed format specification (presentation)
    // stored in this formatter.
    template <typename FormatContext>
    auto format(const CE::Name& name, FormatContext& ctx) const -> decltype(ctx.out()) {
        // ctx.out() is an output iterator to write to.
        return fmt::format_to(ctx.out(), "{}", name.GetString());
    }
};

