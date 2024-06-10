#pragma once

namespace %(NameSpace)s
{
    CLASS()
    class %(Module_API)s %(Name)s : public FWidget
    {
        CE_CLASS(%(Name)s, FWidget)
    public:

        %(Name)s();

    protected:

        void Construct() override;

        FUSION_WIDGET;
    };
    
}

#include "%(Name)s.rtti.h"
