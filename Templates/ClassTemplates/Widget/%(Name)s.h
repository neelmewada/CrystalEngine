#pragma once

namespace %(NameSpace)s
{
    CLASS()
    class %(Module_API)s %(Name)s : public FWidget
    {
        CE_CLASS(%(Name)s, FWidget)
    public:

        // - Public API -

    protected:

        %(Name)s();

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "%(Name)s.rtti.h"
