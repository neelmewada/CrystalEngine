#pragma once

namespace CE
{
    CLASS()
    class %(Module_API)s %(Name)s : public Object
    {
        CE_CLASS(%(Name)s, Object)
    public:

        virtual ~%(Name)s();

    protected:

        %(Name)s();

    };
    
} // namespace CE

#include "%(Name)s.rtti.h"
