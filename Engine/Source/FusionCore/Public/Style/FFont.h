#pragma once

namespace CE
{
    STRUCT()
    struct FUSIONCORE_API FFont
    {
        CE_STRUCT(FFont)
    public:

        virtual ~FFont() = default;

        FFont()
        {}

        FFont(const Name& family, int fontSize = 13) : family(family), fontSize(fontSize)
        {}

        const Name& GetFamily() const { return family; }

        void SetFamily(const Name& family) { this->family = family; }

        int GetFontSize() const { return fontSize; }

        void SetFontSize(int fontSize) { this->fontSize = fontSize; }

    private:

        FIELD()
        Name family = "";

        FIELD()
        int fontSize = 12;

    };
    
} // namespace CE

#include "FFont.rtti.h"