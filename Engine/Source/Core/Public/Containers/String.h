#pragma once

#include "Memory/FixedSizeAllocator.h"

#ifndef STRING_BUFFER_SIZE
#define STRING_BUFFER_SIZE 32
#endif

#ifndef STRING_BUFFER_GROW_COUNT
#define STRING_BUFFER_GROW_COUNT 32
#endif

#include <string>
#include <ostream>

namespace CE
{
	class CORE_API String
	{
	public:

        String();
        String(std::string string);
        String(u32 reservedSize);
        String(const char* value);
        String(const char* cStringA, const char* cStringB);

        String(String&& move) noexcept;
        String(const String& copy);
        String& operator=(const String& rhs);
        String& operator=(const std::string& rhs);
        String& operator=(const char* cString);

        // Concatenate operators
        inline String& operator+(const String& other)
        {
            this->Concatenate(other);
            return *this;
        }

        inline String& operator+=(const String& other)
        {
            this->Concatenate(other);
            return *this;
        }
        inline String& operator+(const char* cString)
        {
            this->ConcatenateCString(cString);
            return *this;
        }
        inline String& operator+=(const char* cString)
        {
            this->ConcatenateCString(cString);
            return *this;
        }

        inline String& operator+(s64 integer)
        {
            this->Concatenate(integer);
            return *this;
        }
        inline String& operator+=(s64 integer)
        {
            this->Concatenate(integer);
            return *this;
        }
        inline operator std::string() const
        {
            return std::string(Buffer);
        }

        inline bool operator<(const String& other) const
        {
            return StringLength < other.StringLength;
        }
        inline bool operator==(const String& other) const
        {
            return strcmp(GetCString(), other.GetCString()) == 0;
        }
        inline bool operator!=(const String& other) const
        {
            return !(*this == other);
        }
        inline bool operator==(const char* cString) const
        {
            if (cString == nullptr)
                return false;
            return strcmp(GetCString(), cString) == 0;
        }
        inline bool operator!=(const char* CString) const
        {
            return !(*this == CString);
        }

        inline char& operator[](u32 index)
        {
            return Buffer[index];
        }
        inline char operator[](u32 index) const
        {
            return Buffer[index];
        }

        friend inline std::ostream& operator<<(std::ostream& os, const String& string)
        {
            os << string.GetCString();
            return os;
        }

        ~String();

        /*
         *  Main Functions
         */

         // Reserves space for the given number of characters in string.
        void Reserve(u32 reserveCharacterCount);

        const char* GetCString() const;
        inline u32 GetLength() const { return StringLength; }

        inline std::string_view ToStringView()
        {
            return std::string_view(GetCString(), StringLength);
        }

        inline void Clear()
        {
            SetCString(nullptr);
        }

        bool IsEmpty() const { return StringLength == 0; }

        /*
         *  Iterators
         */

        struct Iterator
        {
            using iterator_category = std::contiguous_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = char;
            using pointer = char*;  // or also value_type*
            using reference = char&;  // or also value_type&

            Iterator(pointer Ptr) : Ptr(Ptr) {}

            // De-reference ops
            reference operator*() const { return *Ptr; }
            pointer operator->() { return Ptr; }

            // Increment ops
            Iterator& operator++() { Ptr++; return *this; }
            Iterator operator++(int) { Iterator temp = *this; ++(*this); return temp; }
            // Decrement ops
            Iterator& operator--() { Ptr--; return *this; }
            Iterator operator--(int) { Iterator temp = *this; --(*this); return temp; }

            friend bool operator== (const Iterator& a, const Iterator& b) { return a.Ptr == b.Ptr; };
            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.Ptr != b.Ptr; };

        private:
            pointer Ptr;
        };

        Iterator begin() { return Iterator{ &Buffer[0] }; }
        Iterator end() { return Iterator{ &Buffer[0] + StringLength }; }

        Iterator Begin() { return begin(); }
        Iterator End() { return end(); }

        /*
         *  Helper/Utility
         */

        template<typename NativeType>
        inline static String FromNativeType(NativeType value)
        {
            return String(std::to_string(value));
        }

        void Concatenate(const String& string);
        void ConcatenateCString(const char* cString);
        void Concatenate(s64 integer);


    private:

        void SetCString(const char* cString);
        void CopyCString(const char* cString, u32 copyStringLength);

		char* Buffer = nullptr;
		u32 Capacity = 0; // Size of dynamic buffer in bytes

		bool bIsUsingDynamicBuffer = false;
		u32 StringLength = 0;

		static FixedSizeFreeListAllocator<STRING_BUFFER_SIZE, STRING_BUFFER_GROW_COUNT> StaticBufferAllocator;
	};
}
