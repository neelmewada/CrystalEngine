#pragma once

namespace CE::Widgets
{

    struct CFont
    {
    public:

        CFont()
        {}

        CFont(const Name& family, f32 size = 14, bool bold = false)
    	: family(family), size(size), bold(bold)
        {}

        void SetFamily(const Name& family) { this->family = family; }

        void SetSize(f32 size) { this->size = size; }

    	void SetBold(bool set) { this->bold = set; }

        Name GetFamily() const { return family; }

        u32 GetSize() const { return size; }

        bool IsBold() const { return bold; }

    private:

        Name family = "";
        f32 size = 14;
        bool bold = false;

        friend class CPainter;
    };
    
} // namespace CE::Widgets
