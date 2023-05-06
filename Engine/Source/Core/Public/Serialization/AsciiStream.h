
#include "Stream.h"

namespace CE
{

    class CORE_API AsciiStream : public Stream
    {
    public:

        Stream& operator<<(const String& string) override;

        /**
         * \brief Reads one line into the string
         */
        Stream& operator>>(String& string) override;

        virtual Stream& operator<<(const u8& integer)
        {
            return *this << String::Format("{}", integer);
        }

        virtual Stream& operator<<(const u16& integer)
        {
            return *this << String::Format("{}", integer);
        }

        virtual Stream& operator<<(const u32& integer)
        {
            return *this << String::Format("{}", integer);
        }

        virtual Stream& operator<<(const u64& integer)
        {
            return *this << String::Format("{}", integer);
        }

        virtual Stream& operator<<(const s8& integer)
        {
            return *this << String::Format("{}", integer);
        }

        virtual Stream& operator<<(const s16& integer)
        {
            return *this << String::Format("{}", integer);
        }

        virtual Stream& operator<<(const s32& integer)
        {
            return *this << String::Format("{}", integer);
        }

        virtual Stream& operator<<(const s64& integer)
        {
            return *this << String::Format("{}", integer);
        }
        
        bool IsAsciiStream() const override
        {
            return true;
        }
        
        bool IsBinaryStream() const override
        {
            return false;
        }
    };

    
} // namespace CE

