#pragma once

#if PAL_TRAIT_BUILD_TESTS
class FusionCore_Layout_Test;
#endif

namespace CE
{
    CLASS()
    class FUSIONCORE_API FWidget : public Object
    {
        CE_CLASS(FWidget, Object)
    public:

        FWidget();

        virtual ~FWidget();

    protected:

        virtual void Construct();

    private:  // - Fields -

        FIELD()
        Vec4 m_Padding{};

        FIELD()
        Array<FWidget*> m_Children{};

    public:  // - Properties -

        

        Self& Name(const Name& newName)
        {
            this->SetName(newName);
            return *this;
        }

        

    private:

#if PAL_TRAIT_BUILD_TESTS
		class ::FusionCore_Layout_Test;
#endif
    };
    
} // namespace CE

#include "FWidget.rtti.h"
