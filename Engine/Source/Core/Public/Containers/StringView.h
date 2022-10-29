#pragma once

#include "Types/CoreTypeDefs.h"
#include "Algorithm/Hash.h"

#include "fmt/format.h"

#include <string_view>

namespace CE
{
	class CORE_API String;

	class CORE_API StringView
	{
	public:
		StringView() : Impl()
		{}

		StringView(const char* const cString) : Impl(cString)
		{}

		StringView(const char* const cString, const SIZE_T count) : Impl(cString, count)
		{}

		StringView(const String& string);

		auto begin() { return Impl.begin(); }
		auto end() { return Impl.end(); }

		auto Begin() { return begin(); }
		auto End() { return end(); }

		inline bool IsEmpty() 
		{
			return Impl.empty();
		}

		inline SIZE_T GetSize()
		{
			return Impl.size();
		}

		const char* GetCString() const
		{
			return Impl.data();
		}

		// Operators
		bool operator==(const String& rhs);
		bool operator!=(const String& rhs);

		inline bool operator==(const char* rhs)
		{
			return Impl == rhs;
		}

		inline bool operator!=(const char* rhs)
		{
			return Impl != rhs;
		}

	private:
		std::string_view Impl;
	};

} // namespace CE


/// fmt user-defined Formatter for CE::String
template <> struct fmt::formatter<CE::StringView> {
	// Parses format specifications of the form
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
		// Return an iterator past the end of the parsed range:
		return ctx.end();
	}

	// Formats the point p using the parsed format specification
	// stored in this formatter.
	template <typename FormatContext>
	auto format(const CE::StringView& stringView, FormatContext& ctx) const -> decltype(ctx.out()) {
		// ctx.out() is an output iterator to write to.
		return fmt::format_to(ctx.out(), "{}", stringView.GetCString());
	}
};
