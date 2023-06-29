#pragma once

namespace CE::Widgets
{

    ENUM()
    enum class CStackDirection
    {
        Horizontal,
        Vertical
    };
    
    CLASS()
    class COREWIDGETS_API CStackLayout : public CWidget
    {
        CE_CLASS(CStackLayout, CWidget)
    public:
        
        CStackLayout();
        virtual ~CStackLayout();
        
        virtual bool IsContainer() override { return true; }
        
        CStackDirection GetDirection();
        
        void SetDirection(CStackDirection direction);
        
    protected:
        
        void OnDrawGUI() override;

        void HandleEvent(CEvent* event) override;
        
    };

} // namespace CE::Widgets

#include "CStackLayout.rtti.h"
