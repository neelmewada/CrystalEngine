#pragma once

namespace %(NameSpace)s
{
    CLASS()
    class %(Module_API)s %(Name)s : public CWidget
    {
        CE_CLASS(%(Name)s, CWidget)
    public:

        %(Name)s();

        virtual ~%(Name)s();

    protected:

        void Construct() override;

    };
    
}

#include "%(Name)s.rtti.h"
