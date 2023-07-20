#pragma once

namespace CE::Widgets
{

    CLASS()
    class %(Module_API)s %(Name)s : public CWidget
    {
        CE_CLASS(%(Name)s, CWidget)
    public:

        %(Name)s();

        virtual ~%(Name)s();

    protected:

        virtual void OnDrawGUI() override;

    };
    
} // namespace CE

#include "%(Name)s.rtti.h"
