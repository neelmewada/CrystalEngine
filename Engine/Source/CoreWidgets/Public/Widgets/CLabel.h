#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CLabel : public CWidget
    {
        CE_CLASS(CLabel, CE::Widgets::CWidget)
    public:
        
        CLabel();
        virtual ~CLabel();
        
        void SetText(const String& text);
        
        const String& GetText();
        
    protected:
        
        void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;
        
    private:
        
        FIELD()
        String text{};
        
        String invisibleButtonId{};
    };
    
} // namespace CE::Widgets

#include "CLabel.rtti.h"
