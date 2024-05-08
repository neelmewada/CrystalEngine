#pragma once

namespace CE::RHI
{
	template<typename T>
	using Ptr = IntrusivePtr<T>;

	/// @brief Handle is a wrapper around an integral type, where -1 or max value for unsigned type are reserved for null values
	/// @tparam T 
	template<typename T = u32> requires TIsIntegralType<T>::Value
	class Handle
	{
	public:
		using IndexType = T;

		static constexpr T NullValue = T(-1);

		Handle() : value(NullValue)
		{

		}

		Handle(T value) : value(value)
		{

		}

		inline operator T() const
		{
			return value;
		}

		inline bool IsNull() const
		{
			return value == NullValue;
		}

		inline bool IsValid() const
		{
			return !IsNull();
		}

		inline T Get() const
		{
			return value;
		}

		inline void Reset()
		{
			value = NullValue;
		}

		SIZE_T GetHash() const
		{
			return CE::GetHash<T>(value);
		}

	private:
		T value = NullValue;
	};
    
} // namespace CE::RPI
