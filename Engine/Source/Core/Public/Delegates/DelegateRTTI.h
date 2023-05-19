#pragma once

namespace CE
{
	template<typename T>
	struct TIsDelegate : TFalseType 
	{
		static constexpr bool IsSingleCast = false;
		static constexpr bool IsMultiCast = false;
	};

	template<typename T>
	struct TIsDelegate<Delegate<T>> : TTrueType
	{
		static constexpr bool IsSingleCast = true;
		static constexpr bool IsMultiCast = false;
	};

	template<typename T>
	struct TIsDelegate<MultiCastDelegate<T>> : TTrueType
	{
		static constexpr bool IsSingleCast = false;
		static constexpr bool IsMultiCast = true;
	};
    
} // namespace CE

