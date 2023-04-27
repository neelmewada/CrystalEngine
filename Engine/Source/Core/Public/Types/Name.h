#pragma once

#include "Algorithm/Hash.h"
#include "Containers/String.h"
#include "Containers/StringView.h"

#define YAML_CPP_API
#include "yaml-cpp/yaml.h"

namespace CE
{
    class String;
    class StringView;

    template<typename ElementType>
    class Array;

    /// A type used to define path to Reflected c++ types & members
    /// Ex: ParentNamespace::ChildNamespace::SomeClass
    class CORE_API Name
    {
    public:
        Name() : Name("")
        {}
        
        Name(String name);
        Name(const char* name);
        ~Name() = default;

        CE_INLINE bool IsValid() const
        {
            return !value.IsEmpty() && hashValue != 0;
        }

        CE_INLINE SIZE_T GetHashValue() const { return hashValue; }

        CE_INLINE String GetString() const { return value; }
        
        CE_INLINE const char* GetCString() const
        {
            return value.GetCString();
        }

        CE_INLINE bool operator==(const Name& rhs) const
        {
            return hashValue == rhs.hashValue;
        }

        CE_INLINE bool operator!=(const Name& rhs) const
        {
            return hashValue != rhs.hashValue;
        }

        void GetComponents(CE::Array<String>& components) const;
        
        String GetLastComponent() const;

    private:
        String value;
        SIZE_T hashValue;
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

namespace YAML 
{
    inline Emitter& operator<<(Emitter& emitter, CE::Name name) {
        return emitter.Write(std::string(name.GetString().GetCString()));
    }
}
