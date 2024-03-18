#pragma once

namespace CE::Widgets
{

    class CFont
    {
    public:

        CFont()
        {}

        CFont(const Name& family, u32 size = 14, bool bold = false)
    	: family(family), size(size), bold(bold)
        {}

        void SetFamily(const Name& family) { this->family = family; }

        void SetSize(u32 size) { this->size = size; }

    	void SetBold(bool set) { this->bold = set; }

    private:

        Name family = "";
        u32 size = 14;
        bool bold = false;

        friend class CPainter;
    };
    
} // namespace CE::Widgets
